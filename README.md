# DiceRollGraveler

This project was written in response to a video by Shoddycast:
https://www.youtube.com/watch?v=M8C8dHQE2Ro

It simulates rolling a dice a given number of times for a given number of sessions per thread. In the end we print the number the said dice rolled a one.
My goal was to keep the code as similar to the one in the video, but levereage the efficiency of C++ and utilize multithreading to make it run faster.

When I ran it on a 6 core CPU with 12 threads (Ryzen 5 4600H) for a total of 1,008,000,000 sessions the program executed in 202s: ![Screenshot](https://github.com/user-attachments/assets/171c0ca5-7016-4fe6-af0e-7c8f7c7f74a0)

All the code is in a single file called main.cpp
