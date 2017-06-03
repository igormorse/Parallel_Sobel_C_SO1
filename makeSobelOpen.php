<?php

$executable = penmp;

// Define o TimeZone para o do Brasil.
date_default_timezone_set("Brazil/East");

chdir(sobel_thread_open);

echo `gcc -fopenmp sobel.c libimg.c -o $executable -lm`;

$now = new DateTime('now');

$now = $now->format('Y-m-d_H-i-s');

$defaultImage = "thc";


if (empty($arg[0]))
    $defaultInput = "data/$defaultImage.ppm";
else 
    $defaultInput = "data/$arg[0].ppm";

if (empty($arg[1]))
    $defaultOutput = "output/{$defaultImage}_$now";
else
    $defaultOutput = "output/{$arg[1]}_$now";

$ppmOutput = "$defaultOutput.ppm";

$output = `./$executable $defaultInput $ppmOutput`;

$jpgOutput = "$defaultOutput.jpg";

echo "\n\n------------ IMAGE CONVERT PPM TO JPG BEGIN ----------------------\n\n";

echo `convert $ppmOutput $jpgOutput`;

echo "\n\n------------ IMAGE CONVERT PPM TO JPG ENDED ----------------------\n\n";

die($output);