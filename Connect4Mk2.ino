byte board[6][2] = {
  {0b0000000, 0b0000000},
  {0b0000000, 0b0000000},
  {0b0000000, 0b0000000},
  {0b0000000, 0b0000000},
  {0b0000000, 0b0000000},
  {0b0000111, 0b1110000}
};

//   0123456
//  0.......
//  1.......
//  2.......
//  3.......
//  4.......
//  5.......

unsigned int score[2] = {1000, 1000};

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
    Serial.println();
  }
  sprintf(buf, "\nScores: %c = %d, %c = %d\n", token[0], score[0], token[1], score[1]);
  Serial.println(buf);
  
}

void evaluateLine(byte board[6][2], byte player, char posRow, char posCol, char rowDir, char colDir) {
  
  byte playerScore = 0;
  byte opponentScore = 0;
  for (byte pos = 0; pos < 4; pos++) {
    if (board[posRow][player] & mask[posCol]) playerScore++;
    if (board[posRow][1 - player] & mask[posCol]) opponentScore++;
    posRow += rowDir;
    posCol += colDir;
  }
  sprintf(buf, "%c has %d counters, %c has %d counters, ", token[player], playerScore, token[1 - player], opponentScore);
  Serial.print(buf);
  if (opponentScore > 0 && playerScore > 0) {
    sprintf(buf, "no change");
  }
  else if (opponentScore > 0 && playerScore == 0) {
    sprintf(buf, "%c would loose %d points", token[1 - player], points[opponentScore]);
  }
  else if (opponentScore == 0) {
    sprintf(buf, "%c would gain %d points", token[player], points[playerScore + 1] - points[playerScore]);
  }
  Serial.println(buf);
  
}

void evaluateLines(byte board[6][2], byte player, char startRow, char startCol, char rowDir, char colDir,
                   char minRow, char maxRow, char minCol, char maxCol) {

  for (byte line = 0; line <= 3; line++) {
    if (startRow >= minRow && startRow <= maxRow && startCol >= minCol && startCol <= maxCol) {
      byte dirNbr = 0;
      if (rowDir != 0)
        if (colDir == 0) dirNbr = 1;
        else if (colDir == 1) dirNbr = 2;
        else dirNbr = 3;
      sprintf(buf, "checking %s line from col %d row %d: ", dirName[dirNbr], startCol, startRow);
      Serial.print(buf);
      evaluateLine(board, player, startRow, startCol, rowDir, colDir);
    }
    startRow -= rowDir;
    startCol -= colDir;
  }

}

void evaluateMove(byte board[6][2], byte player, byte col) {
  
  byte row = 0;
  while ((row < 5) && ((board[row + 1][0] & mask[col]) == 0) && ((board[row + 1][1] & mask[col]) == 0)) row++;
  sprintf(buf, "%c's counter would land in row %d column %d", token[player], row, col);
  Serial.println(buf);

  //Check win lines
  evaluateLines(board, player, row, col, 0, 1, 0, 5, 0, 3);
  evaluateLines(board, player, row, col, 1, 0, 0, 2, 0, 6);
  evaluateLines(board, player, row, col, 1, 1, 0, 2, 0, 3);
  evaluateLines(board, player, row, col, 1, -1, 0, 2, 3, 6);
  
}

void setup() {
  
  Serial.begin(115200);
  printBoard();
  evaluateMove(board, 0, 3);
  
}

void loop() {

}
