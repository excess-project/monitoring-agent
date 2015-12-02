#!/bin/bash

rm -rf www
mkdir www

pandoc -s -S -H monitoring-guide.css monitoring-guide.md -o ./www/monitoring-guide.html
pandoc -s -S -H monitoring-guide.css monitoring-guide-cluster.md -o ./www/monitoring-guide-cluster.html
pandoc -s -S -H monitoring-guide.css monitoring-guide-plugin-tutorial.md -o ./www/monitoring-guide-plugin-tutorial.html
