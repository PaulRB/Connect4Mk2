byte board[6][2] = {
  {0b0000000, 0b0000000},
  {0b0000000, 0b0000000},
  {0b0000000, 0b0000000},
  {0b0000000, 0b0000000},
  {0b0000000, 0b0000000},
  {0b0000000, 0b0000000}
};

//   0123456
//  0.......
//  1.......
//  2.......
//  3.......
//  4.......
//  5.......

unsigned long score[2] = {0, 0};

const char token[2] = {'O', 'X'};
const int points[5] = {0, 1, 100, 1000, 10000};
const byte mask[7] = {0b1000000, 0b0100000, 0b0010000, 0b0001000, 0b0000100, 0b0000010, 0b0000001};

const char dirName[4][6] = {"horz", "vert", "diag\\", "diag/"};
char buf[80];

void printBoard() {
  
  Serial.println(" 0123456");
  for (byte row = 0; row < 6; row++) {
    Serial.print(row);
    for (byte col = 0; col < 7; col++) {
      char c = '.';
      for (byte player = 0; player <= 1; player++)
        if (board[row][player] & mask[col])
          if (c == '.') c = token[player];
          else c = '?';
      Serial.print(c);
    }
    if (row == 1) sprintf(buf, "  Scores:");
    else if (row == 2) sprintf(buf, "  %c = %ld", token[0], score[0]);
    else if (row == 3) sprintf(buf, "  %c = %ld", token[1], score[1]);
    else sprintf(buf, "");
    Serial.println(buf);
  }
  Serial.println();
}

void evaluateLine(byte board[6][2], byte player, char posRow, char posCol, char rowDir, char colDir, long *playerScore, long *opponentScore) {
  
  byte playerTokens = 0;
  byte opponentTokens = 0;
  for (byte pos = 0; pos < 4; pos++) {
    if (board[posRow][player] & mask[posCol]) playerTokens++;
    if (board[posRow][1 - player] & mask[posCol]) opponentTokens++;
    posRow += rowDir;
    posCol += colDir;
  }
  //sprintf(buf, "%c has %d counters, %c has %d counters, ", token[player], playerTokens, token[1 - player], opponentTokens);
  //Serial.print(buf);

  if (opponentTokens > 0 && playerTokens > 0) {
    //sprintf(buf, "no change");
  }
  else if (opponentTokens > 0 && playerTokens == 0) {
    *opponentScore -= points[opponentTokens];
    //sprintf(buf, "%c would loose %d points", token[1 - player], points[opponentTokens]);
  }
  else if (opponentTokens == 0) {
    *playerScore += points[playerTokens + 1] - points[playerTokens];
    //sprintf(buf, "%c would gain %d points", token[player], points[playerTokens + 1] - points[playerTokens]);
  }
  //Serial.println(buf);
  
}

void evaluateLines(byte board[6][2], byte player, char startRow, char startCol, char rowDir, char colDir,
                   char minRow, char maxRow, char minCol, char maxCol, long *playerScore, long *opponentScore) {

  for (byte line = 0; line <= 3; line++) {
    if (startRow >= minRow && startRow <= maxRow && startCol >= minCol && startCol <= maxCol) {
      /*byte dirNbr = 0;
      if (rowDir != 0)
        if (colDir == 0) dirNbr = 1;
        else if (colDir == 1) dirNbr = 2;
        else dirNbr = 3;
      sprintf(buf, "checking %s line from col %d row %d: ", dirName[dirNbr], startCol, startRow);
      Serial.print(buf);
      */
      evaluateLine(board, player, startRow, startCol, rowDir, colDir, playerScore, opponentScore);
    }
    startRow -= rowDir;
    startCol -= colDir;
  }

}

void evaluateMove(byte board[6][2], byte player, byte col, long *playerScore, long *opponentScore) {
  
  byte row = 0;
  while ((row < 5) && ((board[row + 1][0] & mask[col]) == 0) && ((board[row + 1][1] & mask[col]) == 0)) row++;
  //sprintf(buf, "%c's counter would land in row %d column %d", token[player], row, col);
  //Serial.println(buf);

  //Check win lines
  evaluateLines(board, player, row, col, 0, 1, 0, 5, 0, 3, playerScore, opponentScore); // Horizontal
  evaluateLines(board, player, row, col, 1, 0, 0, 2, 0, 6, playerScore, opponentScore); // Vertical
  evaluateLines(board, player, row, col, 1, 1, 0, 2, 0, 3, playerScore, opponentScore); // Diagonal NW to SE
  evaluateLines(board, player, row, col, 1, -1, 0, 2, 3, 6, playerScore, opponentScore); // Diagonal NE to SW

  board[row][player] |= mask[col];
  
}

char bestMove(byte board[6][2], byte player, byte col, long playerScore, long opponentScore, byte lookAhead) {

  // make copy of board & scores
  byte boardCopy[6][2];
  long playerScoreCopy = playerScore;
  long opponentScoreCopy = opponentScore;
  for (char row = 0; row <= 5; row++) {
    for (char player = 0; player <= 1; player++) {
      boardCopy[row][player] = board[row][player];
    }
  }

  // Try all available columns
  for (char col = 0; col <= 6; col++) {
    if boardCopy[0] & mask[col] == 0) {
      evaluateMove(boardCopy, player, col, playerScoreCopy, 
    }
  }
  
}

void setup() {
  
  Serial.begin(115200);
  randomSeed(analogRead(A0));
  
  for (int n = 0; n <= 21; n++) {
    for (byte player = 0; player <= 1; player++) {
      char col;
      do {
        col = random(7);
      } while (board[0][0] & mask[col] || board[0][1] & mask[col]);
      sprintf(buf, "\nRound %d, %c's move, chooses column %d:\n", n, token[player], col);
      Serial.println(buf);
      printBoard();
      evaluateMove(board, player, col, &score[player], &score[1 - player]);
    }
    printBoard();
  } 
}

void loop() {

}
