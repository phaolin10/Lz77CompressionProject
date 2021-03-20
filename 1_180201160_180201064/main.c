#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define OFFSETBITS 6
#define LENGTHBITS (8-OFFSETBITS)

#define OFFSETMASK ((1 << (OFFSETBITS)) - 1)
#define LENGTHMASK ((1 << (LENGTHBITS)) - 1)

#define OFFSETLENGTH(x,y) (x << LENGTHBITS | y)

struct parcalar {
    uint8_t parcaUzunluk;
    char x;
};

struct parcalar *lz77(char *metin, int diziBoyut, int *parcaSayi)
{
    int kapasite = 1 << 3;
    int parcaNumara = 0;
    struct parcalar parca;

    char *gez, *ara;

    struct parcalar *parcalanan = malloc(kapasite * sizeof(struct parcalar));

    for (gez = metin; gez < metin + diziBoyut ; gez++)
    {
        ara = gez - OFFSETMASK;

        if (ara < metin)
            ara = metin;

        int maxEslesen = 0;

        char *enUzunEslesme = gez;

        for (; ara < gez; ara++)
        {
            int boyut = maksimumEslesenBoyut(ara, gez, LENGTHMASK);

            if (boyut > maxEslesen)
            {
                maxEslesen = boyut;
                enUzunEslesme = ara;
            }
        }

        if (gez + maxEslesen >= metin + diziBoyut)
        {
            maxEslesen = metin + diziBoyut - gez - 1;
        }

        parca.parcaUzunluk = OFFSETLENGTH(gez - enUzunEslesme, maxEslesen);
        gez += maxEslesen;
        parca.x = *gez;

        if (parcaNumara + 1 > kapasite)
        {
            kapasite = kapasite << 1;
            parcalanan = realloc(parcalanan, kapasite * sizeof(struct parcalar));
        }

        parcalanan[parcaNumara++] = parca ;
    }

    if (parcaSayi)
        *parcaSayi = parcaNumara;

    return parcalanan;
}

inline int maksimumEslesenBoyut(char *s1, char *s2, int limit)
{
    int len = 0;

    while (*s1++ == *s2++ && len < limit)
        len++;

    return len;
}

char *dosyaOku(FILE *f, int *boy)
{
    char *icerik;
    fseek(f, 0, SEEK_END);
    *boy = ftell(f);
    icerik = malloc(*boy);
    fseek(f, 0, SEEK_SET);
    fread(icerik, 1, *boy, f);
    return icerik;
}

int main()
{
    FILE *f;
    int metinBoyutu = 8, tokenSayisi;
    char *kaynakMetin = "";
    struct parcalar *parcalanan;

    if (f = fopen("metin.txt", "rb"))
    {
        kaynakMetin = dosyaOku(f, &metinBoyutu);
        fclose(f);
    }



    parcalanan = lz77(kaynakMetin, metinBoyutu, &tokenSayisi);

    if (f = fopen("lz77.txt", "wb"))
    {
        fwrite(parcalanan, sizeof(struct parcalar), tokenSayisi, f);
        fclose(f);
    }



    printf("Orjinal Boyut: %d, Encode Boyutu: %d \n", metinBoyutu, tokenSayisi * sizeof(struct parcalar));
    system("pause");
    return 0;
}
