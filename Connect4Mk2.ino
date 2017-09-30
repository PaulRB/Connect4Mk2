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

char buf[80];

void printBoard() {
  Serial.println(" 0123456");
  for (byte row = 0; row < 6; row++) {
    Serial.print(row);
    for (byte col = 0; col < 7; col++) {
      byte colMask = mask[col];
      if ((board[row][0] & colMask) == 0 && (board[row][1] & colMask) == 0)
        Serial.print(".");
      else if ((board[row][0] & colMask) && (board[row][1] & colMask))
        Serial.print("?");
      else {
        for (byte player = 0; player <= 1; player++) {
          if (board[row][player] & colMask) Serial.print(token[player]);
        }
      }
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
    if (board[posRow][1-player] & mask[posCol]) opponentScore++;
    posRow += rowDir;
    posCol += colDir;
  }
  sprintf(buf, "%c has %d counters, %c has %d counters, ", token[player], playerScore, token[1-player], opponentScore);
  Serial.print(buf);
  if (opponentScore > 0 && playerScore > 0) {
    sprintf(buf, "no change");
  }
  else if (opponentScore > 0 && playerScore == 0) {
    sprintf(buf, "%c would loose %d points", token[1-player], points[opponentScore]);
  }
  else if (opponentScore == 0) {
    sprintf(buf, "%c would gain %d points", token[player], points[playerScore + 1] - points[playerScore]);
  }
  Serial.println(buf);
}

void evaluateMove(byte board[6][2], byte player, byte col) {
  byte row = 0;
  char startCol;
  char startRow;
  while ((row < 5) && ((board[row + 1][0] & mask[col]) == 0) && ((board[row + 1][1] & mask[col]) == 0)) row++;
  sprintf(buf, "%c's counter would land in row %d column %d", token[player], row, col);
  Serial.println(buf);

  //Check Horiz win lines

  startCol = col;
  startRow = row;
  for (char line = 0; line <= 3; line++) {
    if (startCol <= 3) {
      sprintf(buf, "checking horz line from col %d row %d: ", startCol, row);
      Serial.print(buf);
      evaluateLine(board, player, startRow, startCol, 0, 1);
    }
    startCol--;
    if (startCol < 0) break;
  }

  //Check Vert win lines

  startRow = row;
  startCol = col;
  for (byte line = 0; line <= 3; line++) {
    if (startRow <= 2) {
      sprintf(buf, "checking vert line from col %d row %d: ", startCol, startRow);
      Serial.print(buf);
      evaluateLine(board, player, startRow, startCol, 1, 0);
    }
    startRow--;
    if (startRow < 0) break;
  }

  //Check diag\ win lines

  startRow = row;
  startCol = col;
  for (byte line = 0; line <= 3; line++) {
    if (startRow <= 2 && startCol <= 3) {
      sprintf(buf, "checking diag\\ line from col %d row %d: ", startCol, startRow);
      Serial.print(buf);
      evaluateLine(board, player, startRow, startCol, 1, 1);
    }
    startRow--;
    startCol--;
    if (startRow < 0 || startCol < 0) break;
  }

  //Check diag/ win lines

  startRow = row;
  startCol = col;
  for (byte line = 0; line <= 3; line++) {
    if (startRow <= 2 && startCol >= 3) {
      sprintf(buf, "checking diag/ line from col %d row %d: ", startCol, startRow);
      Serial.print(buf);
      evaluateLine(board, player, startRow, startCol, 1, -1);
    }
    startRow--;
    startCol++;
    if (startRow < 0 || startCol > 6) break;
  }
}


void setup() {
  Serial.begin(115200);
  printBoard();
  evaluateMove(board, 0, 3);
}

void loop() {

}
