.segment "CHR"
main_chr:
    .export main_chr
    .incbin "sprites.chr"

.segment "RODATA"
testingnam:
    .export testingnam
    .export _testingnam = testingnam
    .incbin "testingnam.nam"
