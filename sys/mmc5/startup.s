.segment "STARTUP_MAP"
.import famistudio_init
.importzp prgbank
    lda #<.bank(famistudio_init)
    sta prgbank
    sta $5114 ; mmc5 bank set to prgbank
    lda #$03
    sta $5100 ; PRG mode 3, but a bit redundant
    lda #$03
    sta $5101 ; CHR mode 3
.import select_chr_8k_0000
    ; initialize chr banks
    lda #$00
    jsr select_chr_8k_0000
