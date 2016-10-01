#!/bin/sh

git remote add originHTTPS https://github.com/crscillitoe/messenger.git
git commit -m "$1"
git push -u originHTTPS
