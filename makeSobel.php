<?php

// Define o TimeZone para o do Brasil.
date_default_timezone_set("Brazil/East");

chdir(sobel);

echo `gcc sobel.c libimg.c -o sobel -lm`;

$now = new DateTime('now');

$now = $now->format('Y-m-d_H-i-s');

$defaultImage = "escola_artes_visuais";


if (empty($arg[0]))
    $defaultInput = "data/$defaultImage.ppm";
else 
    $defaultInput = "data/$arg[0].ppm";

if (empty($arg[1]))
    $defaultOutput = "output/{$defaultImage}_$now.ppm";
else
    $defaultOutput = "output/{$arg[1]}_$now.ppm";

$output = `./sobel $defaultInput $defaultOutput`;

die($output);