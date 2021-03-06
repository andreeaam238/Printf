/* Blaga Ana-Maria-Andreea, 324CB */

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Define pentru octet. */

#define OCTET 8

/* Define-uri pentru bazele de numeratie in care se doreste afisarea
  numarului. */

#define DECIMAL 10
#define HEXADECIMAL 16

/* Functie care inverseaza un sir de carcatere.
   Se parcurge sirul pana la jumatate si se inverseaza caracterele intre ele:
  primul cu ultimul etc. */

void reverseString(char *string, int length) {
  int i;
  char temp;

  for (i = 0; i < length / 2; i++) {
    temp = string[i];
    string[i] = string[length - i - 1];
    string[length - i - 1] = temp;
  }
}

/* Functie care creeaza un sir de caractere corespunzator unui numar intreg cu
  semn si intoarce lungimea sirului creat. */

int signedToString(int signedNumber, char *string) {
  int temp, length = 0, negative;

  /* Se verifica daca numarul este negativ sau pozitiv. */

  negative = signedNumber < 0;

  /* Se ia numarul cifra cu cifra si se realizeaza conversia acestora in format
    char.
     Se repeta procedeul pana cand nu mai exista cifre din numarul initial.
   */

  do {
    /* Se ia ultima cifra din numar si se inmulteste cu -1 daca numarul este
      negativ, apoi se adauga in sir si se elimina din numar. */

    temp = signedNumber % DECIMAL;
    signedNumber /= DECIMAL;

    temp = (temp < 0) ? -temp : temp;
    string[length] = temp + '0';

    length++;
  } while (signedNumber != 0);

  /* Daca numarul este negativ se adauga caracterul '-' la sfarsitul sirului. */

  if (negative) {
    string[length] = '-';
    length++;
  }

  /* Sirul obtinut anterior este oglinditul numarului initial asa ca se
    apeleaza reverseString.
     Se adauga la finalul sirului '\0'(null terminator) desi nu era nevoie
    neaparat. */

  reverseString(string, length);
  string[length] = '\0';

  return length;
}

/* Functie care creeaza un sir de caractere corespunzator unui numar intreg
  fara semn si intoarce lungimea sirului creat.
   Sirul poate contine codificarea numarului in format zecimal sau
  hexazecimal. */

int unsignedToString(unsigned int unsignedNumber, char *string,
                     unsigned int base) {
  unsigned int temp;
  int length = 0;

  /* Se realizeaza impartirea repetata a numarului la valoarea bazei de
    numeratie in care se doreste afisarea.
    Se repeta procedeul pana cand numarul devine 0. */

  do {
    temp = unsignedNumber % base;

    /* Daca restul impartirii este o cifra atunci se adauga in sir, altfel
      se adauga litera corespunzatoare numarului respectiv:
      10 -> 'a'; 11 -> 'b' etc */

    string[length] = (temp < 10) ? temp + '0' : temp + 'a' - 10;

    length++;
    unsignedNumber /= base;
  } while (unsignedNumber != 0);

  /* Sirul obtinut anterior este oglinditul numarului initial asa ca se
    apeleaza reverseString.
     Se adauga la finalul sirului '\0'(null terminator) desi nu era nevoie
    neaparat. */

  reverseString(string, length);
  string[length] = '\0';

  return length;
}

/* Functie pentru afisarea unui anumit numar de caractere dintr-un sir. */

static int write_stdout(const char *token, int length) {
  int rc;
  int bytes_written = 0;

  do {
    rc = write(1, token + bytes_written, length - bytes_written);
    if (rc < 0)
      return rc;

    bytes_written += rc;
  } while (bytes_written < length);

  return bytes_written;
}

/* Functia primeste ca parametru un sir de formatare(acesta determina ce se 
  afiseaza), creeaza siruri de caractere conform acestuia pe care le afiseaza
  si intoarce numarul de caractere afisate. */

int iocla_printf(const char *format, ...) {
  va_list args;
  char const *ptr;
  char *string, buffer[sizeof(int) * OCTET + 1] = {}, character;
  unsigned int unsignedNumber, base;
  int signedNumber, noPrintedChars = 0, length;

  /* In cazul in care nu exista nimic de afisat se intoarce -1. */

  if (!format) {
    return -1;
  }

  /* Se initializeaza args pentru a prelua parametrii de dupa format */

  va_start(args, format);

  /* Pentru fiecare caracter din format se executa: */

  for (ptr = format; *ptr != '\0'; ptr++) {

    /* Daca se identifica caracterul '%' este posibil ca urmatorul caracter din
      sir sa fie un specificator de conversie, altfel se afiseaza caracterul
      respectiv.
      Se adauga la numarul de caractere afisate lungimea 
      sirului/a caracterului respectiv. */

    if (*ptr == '%') {
      ptr++;
      if (*ptr) {
        switch (*ptr) {

          /* %d intreg, convertit la reprezentare zecimala, cu semn. */

          case 'd':
            signedNumber = va_arg(args, int);
            length = signedToString(signedNumber, buffer);
            write_stdout(buffer, length);
            break;

          /* %u intreg, convertit la reprezentare zecimala, fara semn.
             %x intreg, convertit la reprezentare hexazecimala, fara semn. */

          case 'u':
          case 'x':
            unsignedNumber = va_arg(args, unsigned int);
            base = (*ptr == 'x') ? HEXADECIMAL : DECIMAL;
            length = unsignedToString(unsignedNumber, buffer, base);
            write_stdout(buffer, length);
            break;

          /* %c caracter, convertit la reprezentarea ASCII. */

          case 'c':
            character = (char)va_arg(args, int);
            length = 1;
            write_stdout(&character, length);
            break;

          /* %s pointer la un sir de caractere, ce va fi afisat neschimbat. */

          case 's':
            string = va_arg(args, char *);
            length = strlen(string);
            write_stdout(string, length);
            break;

          /* Daca nu a fost identificat niciun specificator de conversie se
            afiseaza caracterul '%'. */

          default:
            length = 1;
            write_stdout("%", length);
            break;
        }
      }
    } else {
      write_stdout(ptr, 1);
      length = 1;
    }
    noPrintedChars += length;
  }

  va_end(args);
  return noPrintedChars;
}
