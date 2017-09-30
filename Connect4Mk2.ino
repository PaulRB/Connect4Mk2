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

char buf[80];

void printBoard() {
  Serial.println(" 0123456");
  for (byte row = 0; row < 6; row++) {
    Serial.print(row);
    byte mask = 0b1000000;
    for (byte col = 0; col < 7; col++) {
      if ((board[row][0] & mask) && (board[row][1] & mask))
        Serial.print("?");
      else if (board[row][0] & mask)
        Serial.print("O");
      else if (board[row][1] & mask)
        Serial.print("X");
      else
        Serial.print(".");
      mask >>= 1;
    }
    Serial.println();
  }
  sprintf(buf, "\nScores: O = %d, X = %d\n", score[0], score[1]);
  Serial.println(buf);
}

void evaluateMove(byte board[6][2], byte player, byte col) {
  byte mask = 0b1000000 >> col;
  byte row = 0;
  byte opponent = 1 - player;
  while ((row < 5) && ((board[row+1][0] & mask) == 0) && ((board[row+1][1] & mask) == 0)) row++;
  sprintf(buf, "%c's counter would land in row %d column %d", token[player], row, col);
  Serial.println(buf);

  //Check Horiz win lines

  char startCol = col;
  char startRow = row;
  char startMask = mask;
  for (char line = 0; line <= 3; line++) {
    if (startCol <= 3) {
      sprintf(buf, "checking horz line from col %d row %d: ", startCol, row);
      Serial.print(buf);
      byte playerScore = 0;
      byte opponentScore = 0;
      byte posMask = startMask;
      char posRow = startRow;
      for (byte pos = 0; pos < 4; pos++) {
        if (board[posRow][player] & posMask) playerScore++;
        if (board[posRow][opponent] & posMask) opponentScore++;
        posMask >>= 1;
      }
      sprintf(buf, "%c has %d counters, %c has %d counters, ", token[player], playerScore, token[opponent], opponentScore);
      Serial.print(buf);
      if (opponentScore > 0 && playerScore > 0) {
        sprintf(buf, "no change");
      }
      else if (opponentScore > 0 && playerScore == 0) {
        sprintf(buf, "%c would loose %d points", token[opponent], points[opponentScore]);
      }
      else if (opponentScore == 0) {
        sprintf(buf, "%c would gain %d points", token[player], points[playerScore+1] - points[playerScore]);
      }
      Serial.println(buf);
    }
    startCol--;
    startMask <<= 1;
    if (startCol < 0) break;
  }

  //Check Vert win lines

  startRow = row;
  startCol = col;
  startMask = mask;
  for (byte line = 0; line <= 3; line++) {
    if (startRow <= 2) {
      sprintf(buf, "checking vert line from col %d row %d: ", startCol, startRow);
      Serial.print(buf);
      byte playerScore = 0;
      byte opponentScore = 0;
      char posRow = startRow;
      byte posMask = startMask;
      for (byte pos = 0; pos < 4; pos++) {
        if (board[posRow][player] & posMask) playerScore++;
        if (board[posRow][opponent] & posMask) opponentScore++;
        posRow++;
      }
      sprintf(buf, "%c has %d counters, %c has %d counters, ", token[player], playerScore, token[opponent], opponentScore);
      Serial.print(buf);
      if (opponentScore > 0 && playerScore > 0) {
        sprintf(buf, "no change");
      }
      else if (opponentScore > 0 && playerScore == 0) {
        sprintf(buf, "%c would loose %d points", token[opponent], points[opponentScore]);
      }
      else if (opponentScore == 0) {
        sprintf(buf, "%c would gain %d points", token[player], points[playerScore+1] - points[playerScore]);
      }
      Serial.println(buf);
    }
    startRow--;
    if (startRow < 0) break;
  }

  //Check diag\ win lines

  startRow = row;
  startCol = col;
  startMask = mask;
  for (byte line = 0; line <= 3; line++) {
    if (startRow <= 2 && startCol <= 3) {
      sprintf(buf, "checking diag\\ line from col %d row %d: ", startCol, startRow);
      Serial.print(buf);
      byte playerScore = 0;
      byte opponentScore = 0;
      char posRow = startRow;
      char posCol = startCol;
      byte posMask = startMask;
      for (byte pos = 0; pos < 4; pos++) {
        if (board[posRow][player] & posMask) playerScore++;
        if (board[posRow][opponent] & posMask) opponentScore++;
        posRow++;
        posCol++;
        posMask >>= 1;
      }
      sprintf(buf, "%c has %d counters, %c has %d counters, ", token[player], playerScore, token[opponent], opponentScore);
      Serial.print(buf);
      if (opponentScore > 0 && playerScore > 0) {
        sprintf(buf, "no change");
      }
      else if (opponentScore > 0 && playerScore == 0) {
        sprintf(buf, "%c would loose %d points", token[opponent], points[opponentScore]);
      }
      else if (opponentScore == 0) {
        sprintf(buf, "%c would gain %d points", token[player], points[playerScore+1] - points[playerScore]);
      }
      Serial.println(buf);
    }
    startRow--;
    startCol--;
    startMask <<= 1;
    if (startRow < 0 || startCol < 0) break;
  }

  //Check diag/ win lines

  startRow = row;
  startCol = col;
  startMask = mask;
  for (byte line = 0; line <= 3; line++) {
    if (startRow <= 2 && startCol >= 3) {
      sprintf(buf, "checking diag/ line from col %d row %d: ", startCol, startRow);
      Serial.print(buf);
      byte playerScore = 0;
      byte opponentScore = 0;
      char posRow = startRow;
      char posCol = startCol;
      byte posMask = startMask;
      for (byte pos = 0; pos < 4; pos++) {
        if (board[posRow][player] & posMask) playerScore++;
        if (board[posRow][opponent] & posMask) opponentScore++;
        posRow++;
        posCol--;
        posMask <<= 1;
      }
      sprintf(buf, "%c has %d counters, %c has %d counters, ", token[player], playerScore, token[opponent], opponentScore);
      Serial.print(buf);
      if (opponentScore > 0 && playerScore > 0) {
        sprintf(buf, "no change");
      }
      else if (opponentScore > 0 && playerScore == 0) {
        sprintf(buf, "%c would loose %d points", token[opponent], points[opponentScore]);
      }
      else if (opponentScore == 0) {
        sprintf(buf, "%c would gain %d points", token[player], points[playerScore+1] - points[playerScore]);
      }
      Serial.println(buf);
    }
    startRow--;
    startCol++;
    startMask >>= 1;
    if (startRow < 0 || startCol > 6) break;
  }
}


void setup() {
  Serial.begin(115200);
  printBoard();
  evaluateMove(board, 0, 2);
}

void loop() {

}
