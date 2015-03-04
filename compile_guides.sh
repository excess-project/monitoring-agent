#!/bin/bash
pandoc -s -S -H monitoring-guide.css monitoring-guide.md -o monitoring-guide.html
pandoc -s -S -H monitoring-guide.css monitoring-guide-cluster.md -o monitoring-guide-cluster.html
