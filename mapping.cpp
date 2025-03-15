/*
 * Maps hex topology onto LED's
 * (C) Voidstar Lab LLC 2021
 */

#include "mapping.h"

// I accidentally noted these down 1-indexed and I'm too tired to adjust them
#define headof(S) ((S - 1) * 14)
#define tailof(S) (headof(S) + 13)

/*
Segment number connected to each node face
Beam 0 is at 12:00 and advance clockwise 
-1 means nothing connected on that side
So:
    0 = 12:00
    1 = 2:00
    2 = 4:00
    3 = 6:00
    4 = 8:00
    5 = 10:00
*/

int nodeConnections[25][6] = {
    {-1, -1, 1, -1, 0, -1}, // 0
    {-1, -1, 3, -1, 2, -1}, // 1
    {-1, -1, 5, -1, 4, -1}, // 2
    {-1, 0, 6, 12, -1, -1}, // 3
    {-1, 2, 8, 14, 7, 1},   // 4

    {-1, 4, 10, 16, 9, 3},    // 5
    {-1, -1, -1, 18, 11, 5},  // 6
    {-1, 7, -1, 13, -1, 6},   // 7
    {-1, 9, -1, 15, -1, 8},   // 8
    {-1, 11, -1, 17, -1, 10}, // 9

    {12, -1, 19, -1, -1, -1}, // 10
    {14, -1, 21, -1, 20, -1}, // 11
    {16, -1, 23, -1, 22, -1}, // 12
    {18, -1, -1, -1, 24, -1}, // 13
    {13, 20, 25, 29, -1, -1}, // 14

    {15, 22, 27, 31, 26, 21}, // 15
    {17, 24, -1, 33, 28, 23}, // 16
    {-1, 26, -1, 30, -1, 25}, // 17
    {-1, 28, -1, 32, -1, 27}, // 18
    {29, -1, 34, -1, -1, -1}, // 19

    {31, -1, 36, -1, 35, -1},  // 20
    {33, -1, -1, -1, 37, -1},  // 21
    {30, 35, 38, -1, -1, 34},  // 22
    {32, 37, -1, -1, 39, 36},  // 23
    {-1, 39, -1, -1, -1, 38}}; // 24

// First member: Node closer to ceiling
// Second: Node closer to floor
int segmentConnections[40][2] = {
    {0, 3},    // 0
    {0, 4},    // 1
    {1, 4},    // 2
    {1, 5},    // 3
    {2, 5},    // 4
    {2, 6},    // 5
    {3, 7},    // 6
    {4, 7},    // 7
    {4, 8},    // 8
    {5, 8},    // 9
    {5, 9},    // 10
    {6, 9},    // 11
    {3, 10},   // 12
    {7, 14},   // 13
    {4, 11},   // 14
    {8, 15},   // 15
    {5, 12},   // 16
    {9, 16},   // 17
    {6, 13},   // 18
    {10, 14},  // 19
    {11, 14},  // 20
    {11, 15},  // 21
    {12, 15},  // 22
    {12, 16},  // 23
    {13, 16},  // 24
    {14, 17},  // 25
    {15, 17},  // 26
    {15, 18},  // 27
    {16, 18},  // 28
    {14, 19},  // 29
    {17, 22},  // 30
    {15, 20},  // 31
    {18, 23},  // 32
    {16, 21},  // 33
    {19, 22},  // 34
    {20, 22},  // 35
    {20, 23},  // 36
    {21, 23},  // 37
    {22, 24},  // 38
    {23, 24}}; // 39

// First member: Strip number
// Second: LED index closer to ceiling
// Third: LED index closer to floor
int ledAssignments[40][3] = {
    {2, headof(3), tailof(3)},
    {2, tailof(2), headof(2)},
    {1, headof(10), tailof(10)},
    {1, tailof(9), headof(9)},
    {1, headof(4), tailof(4)},
    {1, tailof(3), headof(3)},

    {2, tailof(6), headof(6)},
    {3, tailof(11), headof(11)},
    {1, headof(11), tailof(11)},
    {1, tailof(8), headof(8)},
    {1, headof(12), tailof(12)},
    {0, tailof(11), headof(11)},

    {2, headof(4), tailof(4)},
    {3, tailof(10), headof(10)},
    {2, tailof(1), headof(1)},
    {1, tailof(7), headof(7)},
    {1, headof(5), tailof(5)},
    {0, tailof(10), headof(10)},
    {1, tailof(2), headof(2)},

    {2, headof(5), tailof(5)},
    {3, tailof(4), headof(4)},
    {3, headof(5), tailof(5)},
    {0, headof(5), tailof(5)},
    {0, tailof(4), headof(4)},
    {1, tailof(1), headof(1)},

    {3, tailof(9), headof(9)},
    {0, headof(6), tailof(6)},
    {1, tailof(6), headof(6)},
    {0, tailof(9), headof(9)},

    {3, tailof(3), headof(3)},
    {3, tailof(8), headof(8)},
    {3, headof(6), tailof(6)},
    {0, tailof(8), headof(8)},
    {0, tailof(3), headof(3)},

    {3, tailof(2), headof(2)},
    {3, headof(7), tailof(7)},
    {0, headof(7), tailof(7)},
    {0, tailof(2), headof(2)},

    {3, tailof(1), headof(1)},
    {0, tailof(1), headof(1)}};

// Border nodes are on the very edge of the network.
// Ripples fired here don't look very impressive.
int numberOfBorderNodes = 10;
int borderNodes[] = {0, 1, 2, 3, 6, 10, 13, 19, 21, 24};

// Cube nodes link three equiangular segments
// Firing ripples that always turn in one direction will draw a cube
int numberOfCubeNodes = 8;
int cubeNodes[] = {7, 8, 9, 11, 12, 17, 18};

// Firing ripples that always turn in one direction will draw a starburst
int starburstNode = 15;

