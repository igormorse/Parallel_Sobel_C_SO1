<?php

$executable = sobelOpenMP;

// Define o TimeZone para o do Brasil.
date_default_timezone_set("Brazil/East");

chdir(sobel);

echo `gcc -o $executable -fopenmp sobelOpenMP.c libimg.c -lm -w`;

$now = new DateTime('now');

$now = $now->format('Y-m-d_H-i-s');

$defaultImage = "escola_artes_visuais";

$numWorkers = 4;

$defaultInput = "data/$defaultImage.ppm";
$defaultOutput = "output/{$defaultImage}_$now";

if (!empty($argv[1]) && is_numeric($argv[1]))
    $numWorkers = $argv[1];
else if (!empty($argv[1]))
    $defaultInput = "data/$argv[1].ppm";

if (empty($argv[2]) && !empty($argv[1]) && !is_numeric($argv[1]))
    $defaultOutput = "output/{$argv[1]}_$now";
else if (!empty($argv[2]))
    $defaultOutput = "output/{$argv[2]}_$now";

$ppmOutput = "$defaultOutput.ppm";

$output = `./$executable $defaultInput $ppmOutput $numWorkers`;

$jpgOutput = "$defaultOutput.jpg";

echo "\n\n------------ IMAGE CONVERT PPM TO JPG BEGIN ----------------------\n\n";

echo `convert $ppmOutput $jpgOutput`;

echo "\n\n------------ IMAGE CONVERT PPM TO JPG ENDED ----------------------\n\n";

die($output);