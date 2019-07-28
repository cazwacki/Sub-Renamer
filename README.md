# Sub-Renamer
Renames subtitle files to maintain filetype while taking the video filename.

This tool is used to convert subtitle files to have identical filenames as their associated videos. Many video players search for a subtitle file with an identical filename to automatically play when a file is selected (ex. VLC, Kodi, XBMC). This tool will find your current subtitles and copy them into your video folder with the correct filename, saving hours of boring file renaming with one command.

TO USE:

Compile with:
gcc -std=c11 -D_BSD_SOURCE -o convert convert.c

Usage:
./convert <video folder> <subtitle folder>

If there are an equal number of subtitle files and video files, the program will sort the videos and subtitles by filename. The association between video and subtitle files will be displayed, prompting the user to continue or exit if it is incorrect. If the user continues, a copy of all the subtitle files will be created with the video file's filename and preserve the subtitle filetype.
