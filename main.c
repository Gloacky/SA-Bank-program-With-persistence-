#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

void clear_screen()
{
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  COORD coord = {0, 0};
  DWORD count;
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(hConsole, &csbi);
  FillConsoleOutputCharacter(hConsole, ' ', csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
  SetConsoleCursorPosition(hConsole, coord);
}

typedef struct
{
  float balance;
  char pin[4];
} UserAccount;

UserAccount load_balance(const char *user_id)
{
  char filename[50];
  snprintf(filename, sizeof(filename), "client_%s.bin", user_id);

  FILE *file = fopen(filename, "rb");
  UserAccount account = {0.0, ""};

  if (file == NULL)
  {
    printf("Aucune fichier de solde trouve pour le client %s. Demarrage avec un solde de 0,00 $.\n", user_id);
    return account;
  }

  if (fread(&account, sizeof(UserAccount), 1, file) != 1)
  {
    printf("Erreur: Impossible de lire les donnees de compte.\n");
    fclose(file);
    return account;
  }

  fclose(file);
  return account;
}

void save_balance(const char *user_id, UserAccount account)
{
  char filename[50];
  snprintf(filename, sizeof(filename), "client_%s.bin", user_id);

  FILE *file = fopen(filename, "wb");

  if (file == NULL)
  {
    printf("Erreur : Impossible de sauvegarder le solde.");
  }

  fwrite(&account, sizeof(UserAccount), 1, file);
  fclose(file);
}

void show_balance(float balance)
{
  printf("*********************\n");
  printf("Votre solde est $%.2f\n", balance);
  printf("*********************\n");
}

void show_balance_alt(float balance)
{
  printf("$%.2f\n", balance);
}

float deposit()
{
  float amount;
  printf("*********************\n");
  printf("Entrez un montant a deposer: ");
  scanf("%f", &amount);
  printf("*********************\n");

  if (amount < 0)
  {
    printf("Montant invalid.\n");
    return 0;
  }
  return amount;
}

float withdraw(float balance)
{
  float amount;
  printf("*********************\n");
  printf("Entrez le montant a retirer: ");
  scanf("%f", &amount);
  printf("*********************\n");

  if (amount > balance)
  {
    printf("Solde insuffisant\n");
    return 0;
  }
  else if (amount < 0)
  {
    printf("Le montant doit etre superieur a 0\n");
    return 0;
  }
  else
  {
    return amount;
  }
}

void save_user_pin(const char *user_id, const char *new_pin)
{
  char filename[50];
  snprintf(filename, sizeof(filename), "client_%s.bin", user_id);

  FILE *file = fopen(filename, "rb+");
  if (file == NULL)
  {
    // printf("Erreur: Fichier introuvable. Creation d'un nouveau fichier.\n");
    file = fopen(filename, "wb+");
    if (file == NULL)
    {
      // printf("Erreur: echec de creation de fichier.\n");
      return;
    }
  }

  UserAccount account;

  if (fread(&account, sizeof(UserAccount), 1, file) != 1)
  {

    // printf("Creating a new account entry.\n");
    account.balance = 0.0;
    strncpy(account.pin, new_pin, sizeof(account.pin) - 1);
    account.pin[sizeof(account.pin) - 1] = '\0';
  }
  else
  {

    strncpy(account.pin, new_pin, sizeof(account.pin) - 1);
    account.pin[sizeof(account.pin) - 1] = '\0';
  }

  fseek(file, 0, SEEK_SET);

  if (fwrite(&account, sizeof(UserAccount), 1, file) != 1)
  {
    printf("Erreur : Echec de l ecriture des donnees du compte.\n");
  }
  else
  {
    printf("Code PIN mis a jour avec succes.\n");
  }

  fclose(file);
}

void load_user_pin(const char *user_id, char *pin_buffer, size_t buffer_size)
{
  char filename[50];
  snprintf(filename, sizeof(filename), "client_%s.bin", user_id);

  FILE *file = fopen(filename, "rb");
  if (file == NULL)
  {
    printf("Erreur: Ficher introuvable\n");
    return;
  }
  UserAccount account;

  if (fread(&account, sizeof(UserAccount), 1, file) != 1)
  {
    printf("Erreur: impossible de lire le fichier\n");
    fclose(file);
    return;
  }

  strncpy(pin_buffer, account.pin, buffer_size - 1);
  pin_buffer[buffer_size - 1] = '\0';

  fclose(file);
}

int is_pin_correct(const char *user_id, const char *input_pin)
{
  char stored_pin[4];
  load_user_pin(user_id, stored_pin, sizeof(stored_pin));

  if (strcmp(stored_pin, input_pin) == 0)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

void main()
{
  clear_screen();
  char user_id[20];
  printf("Tapper votre ID: ");
  scanf("%s", user_id);

  char entered_pin[4];

  char pin[4];

  UserAccount account = load_balance(user_id);

  int attempts = 0;

  while (attempts < 3)
  {
    if (strlen(account.pin) == 0)
    {
      printf("Entrer un nouveau code pin: ");
      scanf("%3s", account.pin);
      save_user_pin(user_id, account.pin);
      printf("votre nouveau PIN : %s\n", account.pin);
      break;
    }
    else
    {
      printf("Entrer votre code PIN: ");
      scanf("%4s", entered_pin);

      if (is_pin_correct(user_id, entered_pin))
      {
        printf("\n   Bienvenue\n");
        break;
      }
      else
      {
        printf("pin incorrecte\n");
        attempts++;
      }
    }
  }

  if (attempts >= 3)
  {
    exit(0);
  }

  bool is_running = true;
  int choice;

  clear_screen();
  printf("*********************\n");
  printf("   Banque SA   \n");
  printf("*********************\n");
  printf("Tappez 1 pour afficher le solde.\n");
  printf("Tappez 2 pour deposer un montant.\n");
  printf("Tappez 3 pour retirer un montant.\n");
  printf("Tappez 4 pour quiter le programme.\n");
  printf("*********************\n");

  while (is_running)
  {
    printf("Entrez votre choix (1-4): ");
    scanf("%d", &choice);

    switch (choice)
    {
    case 1:
      show_balance(account.balance);
      break;

    case 2:
      account.balance += deposit();
      save_balance(user_id, account);
      printf("Solde apres la transaction: ");
      show_balance_alt(account.balance);
      break;

    case 3:
      account.balance -= withdraw(account.balance);
      save_balance(user_id, account);
      printf("Solde apres la transaction: ");
      show_balance_alt(account.balance);
      break;
    case 4:
      is_running = false;
    }
  }
}