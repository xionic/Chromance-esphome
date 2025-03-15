/*
 * Maps hex topology onto LED's
 * (C) Voidstar Lab LLC 2021
 */

#ifndef MAPPING_H_
#define MAPPING_H_

// I accidentally noted these down 1-indexed and I'm too tired to adjust them
#define headof(S) ((S - 1) * 14)
#define tailof(S) (headof(S) + 13)

// Beam 0 is at 12:00 and advance clockwise
// -1 means nothing connected on that side
extern int nodeConnections[25][6];

// First member: Node closer to ceiling
// Second: Node closer to floor
extern int segmentConnections[40][2];

 // First member: Strip number
 // Second: LED index closer to ceiling
 // Third: LED index closer to floor
 extern int ledAssignments[40][3];

 // Border nodes are on the very edge of the network.
 // Ripples fired here don't look very impressive.
 extern int numberOfBorderNodes;
 extern int borderNodes[];

 // Cube nodes link three equiangular segments
 // Firing ripples that always turn in one direction will draw a cube
 extern int numberOfCubeNodes;
 extern int cubeNodes[];

 // Firing ripples that always turn in one direction will draw a starburst
 extern int starburstNode;

#endif