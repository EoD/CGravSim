#!/bin/bash
git log $1 | head -n 1 | sed 's/commit //'
