byte board[6][2] = {
  {0b0000000, 0b0000000},
  {0b0000000, 0b0000000},
  {0b0000000, 0b0000000},
  {0b0000000, 0b0000000},
  {0b0000000, 0b0000000},
  {0b0000111, 0b1110000}
};

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

void evaluate(byte board[6][2], byte player, byte col) {
  byte mask = 0b1000000 >> col;
  byte row = 0;
  byte opponent = 1 - player;
  while ((row < 5) && ((board[row+1][0] & mask) == 0) && ((board[row+1][1] & mask) == 0)) row++;
  sprintf(buf, "%c's counter would land in row %d column %d", token[player], row, col);
  Serial.println(buf);

  //Check Horiz win lines

  byte leftLimit = 0;
  if (col > 2) leftLimit = col - 3;
  byte rightLimit = 3;
  if (col < 3) rightLimit = col;

  for (byte startCol = leftLimit; startCol <= rightLimit; startCol++) {
    sprintf(buf, "checking horz line from col %d to col %d: ", startCol, startCol + 3);
    Serial.print(buf);
    byte playerScore = 0;
    byte opponentScore = 0;
    byte mask = 0b1000000 >> startCol;
    for (byte col = 0; col < 4; col++) {
      if (board[row][player] & mask) playerScore++;
      if (board[row][opponent] & mask) opponentScore++;
      mask >>= 1;
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

  //Check Vert win lines

  byte topLimit = 0;
  if (row > 2) topLimit = row - 3;
  byte bottomLimit = 2;
  if (row < 2) bottomLimit = row;

  for (byte startRow = topLimit; startRow <= bottomLimit; startRow++) {
    sprintf(buf, "checking vert line from row %d to row %d: ", startRow, startRow + 3);
    Serial.print(buf);
    byte playerScore = 0;
    byte opponentScore = 0;
    for (byte row = startRow; row < startRow + 4; row++) {
      if (board[row][player] & mask) playerScore++;
      if (board[row][opponent] & mask) opponentScore++;
    }
    sprintf(buf, "%c has %d counters, %c has %d counters, ", token[player], playerScore, token[opponent], opponentScore);
    Serial.print(buf);
    if (opponentScore > 0 && playerScore > 0) {
      sprintf(buf, "no change");
    }
    if (opponentScore > 0 && playerScore == 0) {
      sprintf(buf, "%c would loose %d points", token[opponent], points[opponentScore]);
    }
    else if (opponentScore == 0) {
      sprintf(buf, "%c would gain %d points", token[player], points[playerScore+1] - points[playerScore]);
    }
    Serial.println(buf);
  }
  
  //Check diag\ win lines

  byte topLimit = 0;
  if (row > 2) topLimit = row - 3;
  byte bottomLimit = 2;
  if (row < 2) bottomLimit = row;

  for (byte startRow = topLimit; startRow <= bottomLimit; startRow++) {
    sprintf(buf, "checking vert line from row %d to row %d: ", startRow, startRow + 3);
    Serial.print(buf);
    byte playerScore = 0;
    byte opponentScore = 0;
    for (byte row = startRow; row < startRow + 4; row++) {
      if (board[row][player] & mask) playerScore++;
      if (board[row][opponent] & mask) opponentScore++;
    }
    sprintf(buf, "%c has %d counters, %c has %d counters, ", token[player], playerScore, token[opponent], opponentScore);
    Serial.print(buf);
    if (opponentScore > 0 && playerScore > 0) {
      sprintf(buf, "no change");
    }
    if (opponentScore > 0 && playerScore == 0) {
      sprintf(buf, "%c would loose %d points", token[opponent], points[opponentScore]);
    }
    else if (opponentScore == 0) {
      sprintf(buf, "%c would gain %d points", token[player], points[playerScore+1] - points[playerScore]);
    }
    Serial.println(buf);
  }
}


void setup() {
  Serial.begin(115200);
  printBoard();
  evaluate(board, 0, 3);
}

void loop() {

}
