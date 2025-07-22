/*
 * poletool.c - Converter for "Pole Chudes" dictionary files (UTF-8 ↔ CP866)
 *
 * Author: Dmitry Gerasimuk, 2025 (based on old code for C# by Genjitsu Gadget Lab)
 * License: MIT License (see LICENSE file)
 *
 * This tool reads and writes POLE.OVL dictionary file used in the classic MS-DOS game
 * "Pole Chudes", converting between UTF-8 and CP866 encodings.
 *
 * Usage:
 *   poletool unpack POLE.OVL > dict.txt      
 *   dicttool pack   dict.txt  > POLE.OVL     
 * 
 * Build:
 * gcc -o poletool poletool.c
 * you may need to use -liconv 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>
#include <errno.h>

#define LINE_LEN 20
 
// CP866 to UTF8 with adjustment for bytes >= 0xB0

char* cp866_to_utf8(const unsigned char* input, size_t in_len) {
    
    // Use static context for repeated usage

    static iconv_t cd = (iconv_t)-1;
    if (cd == (iconv_t)-1) {
        cd = iconv_open("UTF-8", "CP866");
        if (cd == (iconv_t)-1) {
            perror("iconv_open");
            return NULL;
        }
    }

     
    // Buffer allocation  
    unsigned char* prebuf = malloc(in_len);
    if (!prebuf) {
        perror("malloc");
        return NULL;
    }
    memcpy(prebuf, input, in_len);

     
    // Changing

    for (size_t i = 0; i < in_len; i++) {
        if (prebuf[i] >= 0xB0) {
            prebuf[i] += 0x30;
        }
    }

    // Count output length
    size_t out_len = in_len * 4 + 1;
    char* outbuf = malloc(out_len);
    if (!outbuf) {
        perror("malloc");
        free(prebuf);
        return NULL;
    }

    // iconv parameters
    char* inptr = (char*)prebuf;
    char* outptr = outbuf;
    size_t inbytesleft = in_len;
    size_t outbytesleft = out_len;

    
    // do convert
    if (iconv(cd, &inptr, &inbytesleft, &outptr, &outbytesleft) == (size_t)-1) {
        perror("iconv");
        free(prebuf);
        free(outbuf);
        return NULL;
    }

    // Add null terminator
    *outptr = '\0';

  
    free(prebuf);
    return outbuf;
}


// replace @ with space cause i was born like that
void replace_at_with_space(char* str) {
    if (!str) return;
    for (; *str; str++) {
        if (*str == '@') *str = ' ';
    }
}

// utf8 to cp866 conversion with adjustment of bytes >= 0xE0
unsigned char* utf8_to_cp866(const char* input, size_t* out_len) {
    iconv_t cd = iconv_open("CP866", "UTF-8");
    if (cd == (iconv_t)-1) {
        perror("iconv_open");
        return NULL;
    }

    size_t in_len = strlen(input);
    size_t outbuf_size = in_len * 2 + 1;
    unsigned char* outbuf = malloc(outbuf_size);
    if (!outbuf) {
        perror("malloc");
        iconv_close(cd);
        return NULL;
    }
    char* inptr = (char*)input;
    char* outptr = (char*)outbuf;
    size_t inbytesleft = in_len;
    size_t outbytesleft = outbuf_size;

    size_t res = iconv(cd, &inptr, &inbytesleft, &outptr, &outbytesleft);
    if (res == (size_t)-1) {
        perror("iconv");
        free(outbuf);
        iconv_close(cd);
        return NULL;
    }

    size_t result_len = outbuf_size - outbytesleft;
    for (size_t i = 0; i < result_len; i++) {
        if (outbuf[i] >= 0xE0) {
            outbuf[i] -= 0x30;
        }
    }

    *out_len = result_len;
    iconv_close(cd);
    return outbuf;
}

 
// Zero out dst and copy src into it of length len (max LINE_LEN)
void pad_line(unsigned char* dst, const unsigned char* src, size_t len) {
    memset(dst, 0, LINE_LEN);
    if (len > LINE_LEN) len = LINE_LEN;
    memcpy(dst, src, len);
}

void usage() {
    printf("Usage: <unpack|pack> <infile> <outfile>\n");
    printf("Usually: unpack .ovl to .txt or pack .txt to .ovl\n");
}

void unpack(const char* infile, const char* outfile) {
    FILE* in = fopen(infile, "rb");
    FILE* out = fopen(outfile, "w");
    if (!in || !out) {
        perror("fopen");
        exit(1);
    }

    fgetc(in);
    
    unsigned char lbuf[LINE_LEN];
    fread(lbuf, 1, LINE_LEN, in);

    char* header_utf = cp866_to_utf8(lbuf, LINE_LEN);
    if (!header_utf) {
        fprintf(stderr, "Failed to convert header\n");
        fclose(in);
        fclose(out);
        exit(1);
    }
    int dbcount = atoi(header_utf);
    free(header_utf);

    int total = 0;
    char last_key[LINE_LEN * 4] = {0};

    while (!feof(in)) {
        int wlen = fgetc(in);
        if (wlen <= 0 || feof(in)) break;

        unsigned char sbuf[LINE_LEN];
        if (fread(sbuf, 1, LINE_LEN, in) != LINE_LEN) break;

        char* word = cp866_to_utf8(sbuf, wlen);
        if (!word) {
            fprintf(stderr, "Error converting word\n");
            break;
        }
        replace_at_with_space(word);
                wlen = fgetc(in);
        if (wlen <= 0 || feof(in)) {
            free(word);
            break;
        }
        if (fread(sbuf, 1, LINE_LEN, in) != LINE_LEN) {
            free(word);
            break;
        }
        char* key = cp866_to_utf8(sbuf, wlen);
        if (!key) {
            fprintf(stderr, "Error converting key\n");
            free(word);
            break;
        }
        replace_at_with_space(key);

        if (strcmp(key, last_key) != 0) {
            fprintf(out, "[%s]\n", key);
            strncpy(last_key, key, sizeof(last_key));
        }
        fprintf(out, "%s\n", word);

        free(word);
        free(key);
        total++;
    }

    printf("TOTAL: %d\n", total);
    if (dbcount == total)
        printf("Database header count matches\n");
    else
        printf("MISMATCH: header count = %d, actual = %d\n", dbcount, total);

    fclose(in);
    fclose(out);
}

void pack(const char* infile, const char* outfile) {
    FILE* in = fopen(infile, "r");
    FILE* out = fopen(outfile, "wb");

    // Проверка открытия файлов
    if (!in || !out) {
        perror("fopen");
        if (in) fclose(in);
        if (out) fclose(out);
        return;
    }

    char line[1024];
    int totalpairs = 0;

     
    // count pairs: only non-header lines with content

    while (fgets(line, sizeof(line), in)) {
        line[strcspn(line, "\r\n")] = '\0';  // remove newline
        if (line[0] != '[' && strlen(line) > 0) {
            totalpairs++;
        }
    }

    // check minimum pairs
    if (totalpairs < 3) {
        fprintf(stderr, "ERROR: need at least 3 key-value pairs\n");
        fclose(in);
        fclose(out);
        return;
    }

     
    rewind(in);

     
    // build count string

    char countstr[16];
    snprintf(countstr, sizeof(countstr), "%d", totalpairs);

 
    // write string len
    fputc((unsigned char)strlen(countstr), out);

    size_t cp_len;
    unsigned char* cp = utf8_to_cp866(countstr, &cp_len);
    if (!cp) {
        fprintf(stderr, "Error converting count string\n");
        fclose(in);
        fclose(out);
        return;
    }

    unsigned char buf[LINE_LEN];
    pad_line(buf, cp, cp_len);
    fwrite(buf, 1, LINE_LEN, out);
    free(cp);

    char currkey[1024] = {0};
    int entry_count = 0;
    int key_count = 0;

 
    // parse input
    while (fgets(line, sizeof(line), in)) {
        line[strcspn(line, "\r\n")] = '\0';  // no crlf

        // Обработка заголовков
        if (line[0] == '[' && line[strlen(line) - 1] == ']') {
            size_t len = strlen(line);
            if (len >= 2) {
                
                snprintf(currkey, sizeof(currkey), "%s", line + 1);
                currkey[len - 2] = '\0';
                key_count++;
            }
        } 
        // Обработка значений
        else if (strlen(line) > 0) {
            unsigned char* cp_word = utf8_to_cp866(line, &cp_len);
            if (!cp_word) {
                fprintf(stderr, "Error converting word\n");
                break;
            }

            // word len
            fputc((unsigned char)cp_len, out);
            pad_line(buf, cp_word, cp_len);
            fwrite(buf, 1, LINE_LEN, out);
            free(cp_word);

            unsigned char* cp_key = utf8_to_cp866(currkey, &cp_len);
            if (!cp_key) {
                fprintf(stderr, "Error converting key\n");
                break;
            }

            // key len
            fputc((unsigned char)cp_len, out);
            pad_line(buf, cp_key, cp_len);
            fwrite(buf, 1, LINE_LEN, out);
            free(cp_key);

            entry_count++;
        }
    }

    printf("Wrote %d keys, %d key-value pairs\n", key_count, entry_count);

   
    fclose(in);
    fclose(out);
}


int main(int argc, char** argv) {
    if (argc < 4) {
        usage();
        return 1;
    }

    if (strcmp(argv[1], "unpack") == 0) {
        unpack(argv[2], argv[3]);
    } else if (strcmp(argv[1], "pack") == 0) {
        pack(argv[2], argv[3]);
    } else {
        usage();
        return 1;
    }

    return 0;
}