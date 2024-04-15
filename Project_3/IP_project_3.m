inputImage = imread('D:\ImageProcessing_Spring2024\Knee.pgm');

[rows, cols] = size(inputImage);
paddedRows = 2 * rows - 1;
paddedCols = 2 * cols - 1;
cutoffFreq1 = 30; % First cutoff frequency
cutoffFreq2 = 400;  % Second cutoff frequency


paddedImage = zeros(paddedRows, paddedCols);
paddedImage(1:rows, 1:cols) = double(inputImage);


for x = 1:paddedRows
    for y = 1:paddedCols
        paddedImage(x, y) = paddedImage(x, y) * (-1)^(x+y);
    end
end


dftResult = computeDFT2D(paddedImage);


lowPassFilter1 = zeros(paddedRows, paddedCols);
for u = 1:paddedRows
    for v = 1:paddedCols
        distance = sqrt((u - paddedRows/2)^2 + (v - paddedCols/2)^2);
        if distance <= cutoffFreq1
            lowPassFilter1(u, v) = 1;
        end
    end
end


filteredDFT1 = lowPassFilter1 .* dftResult;

inverseDFT1 = computeIDFT2D(filteredDFT1);

inverseDFT2 = computeIDFT2D(filteredDFT2);

outputImageCutoff1 = real(inverseDFT1(1:rows, 1:cols));

outputImageCutoff2 = real(inverseDFT2(1:rows, 1:cols));

figure;
subplot(1, 3, 1);
imshow(uint8(inputImage));
title('Input Image');
subplot(1, 3, 2);
imshow(uint8(outputImageCutoff1));
title('Output Image (Cutoff 1)');
subplot(1, 3, 3);
imshow(uint8(outputImageCutoff2));
title('Output Image (Cutoff 2)');


imwrite(uint8(outputImageCutoff1), 'D:\ImageProcessing_Spring2024\knee_30.pgm');
imwrite(uint8(outputImageCutoff2), 'D:\ImageProcessing_Spring2024\knee_400.pgm');

function F = computeDFT2D(x)
    [M, N] = size(x);
    F = zeros(M, N);
    for u = 1:M
        F(u, :) = computeDFT(x(u, :));
    end
    for v = 1:N
        F(:, v) = computeDFT(F(:, v).');
    end
end

function x = computeIDFT2D(F)
    [M, N] = size(F);
    x = zeros(M, N);
    for m = 1:M
        x(m, :) = computeIDFT(F(m, :));
    end
    for n = 1:N
        x(:, n) = computeIDFT(x(:, n).');
    end
end

function X = computeDFT(x)
    N = length(x);
    X = zeros(1, N);
    for k = 0:N-1
        for n = 0:N-1
            X(k+1) = X(k+1) + x(n+1) * exp(-1i * 2 * pi * k * n / N);
        end
    end
end

function x = computeIDFT(X)
    N = length(X);
    x = zeros(1, N);
    for n = 0:N-1
        for k = 0:N-1
            x(n+1) = x(n+1) + X(k+1) * exp(1i * 2 * pi * k * n / N);
        end
        x(n+1) = x(n+1) / N;
    end
end
