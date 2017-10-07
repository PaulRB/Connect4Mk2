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

char evaluateMove(byte board[6][2], byte player, byte col, long *playerScore, long *opponentScore) {

  byte row = 0;
  while ((row < 5) && ((board[row + 1][0] & mask[col]) == 0) && ((board[row + 1][1] & mask[col]) == 0)) row++;
  //sprintf(buf, "%c's counter would land in row %d column %d", token[player], row, col);
  //Serial.println(buf);

  //Check win lines
  evaluateLines(board, player, row, col, 0, 1, 0, 5, 0, 3, playerScore, opponentScore); // Horizontal
  evaluateLines(board, player, row, col, 1, 0, 0, 2, 0, 6, playerScore, opponentScore); // Vertical
  evaluateLines(board, player, row, col, 1, 1, 0, 2, 0, 3, playerScore, opponentScore); // Diagonal NW to SE
  evaluateLines(board, player, row, col, 1, -1, 0, 2, 3, 6, playerScore, opponentScore); // Diagonal NE to SW

  return row;

}

char bestMove(byte board[6][2], byte player, long *playerScore, long *opponentScore, byte lookAhead) {

  char bestCol = 0;
  long bestScore = 0;
  long bestPlayerScore = 0;
  long bestOpponentScore = 0;

  // Try all available columns
  for (char col = 0; col <= 6; col++) {
    // make copy of current scores
    long playerScoreCopy = *playerScore;
    long opponentScoreCopy = *opponentScore;
    //sprintf(buf, "%c considering playing column %d: ", token[player], col);
    //Serial.print(buf);
    if ((board[0][0] & mask[col]) == 0 && (board[0][1] & mask[col]) == 0) {
      char row = evaluateMove(board, player, col, &playerScoreCopy, &opponentScoreCopy);
      //sprintf(buf, "score %c=%ld, %c=%ld", token[player], playerScoreCopy, token[1-player], opponentScoreCopy);
      //Serial.println(buf);
      //Drop in the counter temporarily
      board[row][player] |= mask[col];
      //printBoard();
      if (lookAhead > 0) {
        //sprintf(buf, "looking ahead %d more moves", lookAhead);
        //Serial.println(buf);
        char opponentBest = bestMove(board, 1 - player, &opponentScoreCopy, &playerScoreCopy, lookAhead - 1);
      }
      // Compare this move to best so far
      if (opponentScoreCopy < 10000 && playerScoreCopy - opponentScoreCopy > bestScore || playerScoreCopy >= 10000) {
        bestPlayerScore = playerScoreCopy;
        bestOpponentScore = opponentScoreCopy;
        bestScore = playerScoreCopy - opponentScoreCopy;
        bestCol = col;
      }
      // remove the temporary counter
      board[row][player] &= ~mask[col];
    }
    else {
      //Serial.println("blocked");
    }
  }
  playerScore = bestPlayerScore;
  opponentScore = bestOpponentScore;
  return bestCol;
}

void setup() {

  Serial.begin(115200);

}

char player = 0;

void loop() {

  printBoard();

  if (score[player] >= 10000) {
    sprintf(buf, "Player %c wins", token[player]);
    Serial.println(buf);
    while (1);
  }
  else if (score[1 - player] < 10000) {

    sprintf(buf, "Evaluating best move for %c: ", token[player]);
    Serial.println(buf);
    unsigned long start = millis();
    char col = bestMove(board, player, &score[player], &score[1 - player], 3);
    sprintf(buf, "best move is %d, time taken %ld", col, millis() - start);
    Serial.println(buf);
    byte row = evaluateMove(board, player, col, &score[player], &score[1 - player]);
    board[row][player] |= mask[col];

  }

  player = 1 - player;

  //Serial.print("Press Enter...");
  //while (Serial.available() == 0);
  //while (Serial.available() > 0 ) Serial.read();

}
