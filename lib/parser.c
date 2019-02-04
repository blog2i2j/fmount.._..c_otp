/*
 *
 *  TOTP: Time-Based One-Time Password Algorithm
 *  Copyright (c) 2017, fmount <fmount@inventati.org>
 *
 *  This software is distributed under MIT License
 *
 *  Compute the hmac using openssl library.
 *  SHA-1 engine is used by default, but you can pass another one,
 *
 *  e.g EVP_md5(), EVP_sha224, EVP_sha512, etc
 *
 */

#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>

#include "parser.h"
#include "utils.h"
#include "gpgmelib.h"


void load_providers(char *fname)
{

    FILE *f;
    size_t len = 1024;

    if (fname == NULL)
        exit(ENOENT);
    f = fopen(fname, "r");
    if (f == NULL)
        exit(ENOENT);
    char *line = NULL;
    while (getline(&line, &len, f) != -1) {
        if (line[0] != '#')
            process_provider(&provider_list, line);
    }

    free(line);

    #ifdef DEBUG

    print(provider_list);

    #endif
}

void load_encrypted_providers(char *fin, char *fingerprint)
{
  gpgme_ctx_t ctx;
  gpgme_error_t err;
  gpgme_data_t in, out;
  gpgme_key_t key[2] = { NULL, NULL };
  gpgme_encrypt_flags_t flags = GPGME_ENCRYPT_ALWAYS_TRUST;

  init_context();

  err = gpgme_new(&ctx);
  gpgme_set_armor(ctx, 1);
  gpgme_set_protocol(ctx, PROTOCOL);

  if (err)
    exit_with_err(err);

  //First step is to select the key ..
  select_key(ctx, fingerprint, &key[0]);
  gpgme_data_t dh = decrypt(fin, ctx, in, out);

  #ifdef DEBUG
  print_gpgme_data(dh);
  #endif

  gpgme_data_seek(dh, 0, SEEK_SET);
  char *buf = (char*)malloc(BUFSIZE*sizeof(char));

  while(gpgme_data_read(dh, buf, BUFSIZE) > 0) {
      process_block(buf);

      /***
       * What's the problem here:
       * 1. Block is a list of lines
       * 2. I need to process line by line in this way:
       *
       * if (line[0] != '#')
       *    process_provider(&provider_list, line);
       * 
       * So  |BLOCK| => | l1 | => while(lines) {
       *                | l2 |      process_provider(..)
       *                | l3 |    }
       *
       * ===> Move the implementation in the utils.h to
       *      avoid the coupling between the parser and
       *      the string manipulation
       */
  }

  /* Release all the resources */
  gpgme_data_release(in);
  gpgme_data_release(dh);
  gpgme_release (ctx);
  free(buf);

}

/*int main(int argc, char **argv)
{

    char *fname = NULL;
    int opt;
    if(argc <= 1) {
        fprintf(stderr, "Provide at least one argument\n");
        return -1;
    }

    while((opt = getopt(argc, argv, "f:v")) != -1 ) {
        switch(opt) {
            case 'f':
              fname = optarg;
              break;
            case 'v':
              break;
            default:
              fprintf(stderr, "Usage: %s [-f fname]\n", argv[0]);
        }
    }

    load_providers(fname);
    return 0;
}*/
