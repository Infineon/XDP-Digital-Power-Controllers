/* ============================================================================
** Copyright (C) 2017 Infineon Technologies AG
** All rights reserved.
** ============================================================================
**
** ============================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ============================================================================
*/
/*
 * ntc_temperature_lut.h
 *
 *  Created on: Oct 10, 2017
 *      Author: dlewis4
 */

#ifndef COMMON_MODULES_TELEMETRY_NTC_TEMPERATURE_LUT_H_
#define COMMON_MODULES_TELEMETRY_NTC_TEMPERATURE_LUT_H_

#include <stdint.h>              /** uint*_t */

/**
 * Look up table for ntc temperature decode.  Index is in adc_codes, values are temperature C
 * Autogenerated by $WORKAREA/bin/create_ntc_array_c.py using shasta_pack ntc table
 */
const int16_t ntc_temp_lut[512] =
{
	149 , //degrees C adc_code = 0
	149 , //degrees C adc_code = 1
	149 , //degrees C adc_code = 2
	149 , //degrees C adc_code = 3
	149 , //degrees C adc_code = 4
	149 , //degrees C adc_code = 5
	149 , //degrees C adc_code = 6
	149 , //degrees C adc_code = 7
	148 , //degrees C adc_code = 8
	148 , //degrees C adc_code = 9
	147 , //degrees C adc_code = 10
	147 , //degrees C adc_code = 11
	146 , //degrees C adc_code = 12
	146 , //degrees C adc_code = 13
	145 , //degrees C adc_code = 14
	145 , //degrees C adc_code = 15
	144 , //degrees C adc_code = 16
	144 , //degrees C adc_code = 17
	143 , //degrees C adc_code = 18
	143 , //degrees C adc_code = 19
	142 , //degrees C adc_code = 20
	142 , //degrees C adc_code = 21
	141 , //degrees C adc_code = 22
	140 , //degrees C adc_code = 23
	140 , //degrees C adc_code = 24
	139 , //degrees C adc_code = 25
	139 , //degrees C adc_code = 26
	138 , //degrees C adc_code = 27
	138 , //degrees C adc_code = 28
	137 , //degrees C adc_code = 29
	137 , //degrees C adc_code = 30
	136 , //degrees C adc_code = 31
	136 , //degrees C adc_code = 32
	135 , //degrees C adc_code = 33
	135 , //degrees C adc_code = 34
	134 , //degrees C adc_code = 35
	134 , //degrees C adc_code = 36
	133 , //degrees C adc_code = 37
	132 , //degrees C adc_code = 38
	132 , //degrees C adc_code = 39
	131 , //degrees C adc_code = 40
	131 , //degrees C adc_code = 41
	130 , //degrees C adc_code = 42
	130 , //degrees C adc_code = 43
	129 , //degrees C adc_code = 44
	128 , //degrees C adc_code = 45
	128 , //degrees C adc_code = 46
	127 , //degrees C adc_code = 47
	127 , //degrees C adc_code = 48
	126 , //degrees C adc_code = 49
	126 , //degrees C adc_code = 50
	125 , //degrees C adc_code = 51
	125 , //degrees C adc_code = 52
	124 , //degrees C adc_code = 53
	124 , //degrees C adc_code = 54
	123 , //degrees C adc_code = 55
	122 , //degrees C adc_code = 56
	121 , //degrees C adc_code = 57
	121 , //degrees C adc_code = 58
	120 , //degrees C adc_code = 59
	119 , //degrees C adc_code = 60
	119 , //degrees C adc_code = 61
	118 , //degrees C adc_code = 62
	117 , //degrees C adc_code = 63
	117 , //degrees C adc_code = 64
	116 , //degrees C adc_code = 65
	116 , //degrees C adc_code = 66
	115 , //degrees C adc_code = 67
	114 , //degrees C adc_code = 68
	114 , //degrees C adc_code = 69
	113 , //degrees C adc_code = 70
	113 , //degrees C adc_code = 71
	112 , //degrees C adc_code = 72
	111 , //degrees C adc_code = 73
	111 , //degrees C adc_code = 74
	110 , //degrees C adc_code = 75
	110 , //degrees C adc_code = 76
	109 , //degrees C adc_code = 77
	109 , //degrees C adc_code = 78
	108 , //degrees C adc_code = 79
	108 , //degrees C adc_code = 80
	107 , //degrees C adc_code = 81
	107 , //degrees C adc_code = 82
	106 , //degrees C adc_code = 83
	106 , //degrees C adc_code = 84
	105 , //degrees C adc_code = 85
	105 , //degrees C adc_code = 86
	105 , //degrees C adc_code = 87
	104 , //degrees C adc_code = 88
	104 , //degrees C adc_code = 89
	103 , //degrees C adc_code = 90
	103 , //degrees C adc_code = 91
	102 , //degrees C adc_code = 92
	102 , //degrees C adc_code = 93
	101 , //degrees C adc_code = 94
	101 , //degrees C adc_code = 95
	101 , //degrees C adc_code = 96
	100 , //degrees C adc_code = 97
	100 , //degrees C adc_code = 98
	99 , //degrees C adc_code = 99
	99 , //degrees C adc_code = 100
	99 , //degrees C adc_code = 101
	98 , //degrees C adc_code = 102
	98 , //degrees C adc_code = 103
	97 , //degrees C adc_code = 104
	97 , //degrees C adc_code = 105
	97 , //degrees C adc_code = 106
	96 , //degrees C adc_code = 107
	96 , //degrees C adc_code = 108
	95 , //degrees C adc_code = 109
	95 , //degrees C adc_code = 110
	95 , //degrees C adc_code = 111
	94 , //degrees C adc_code = 112
	94 , //degrees C adc_code = 113
	94 , //degrees C adc_code = 114
	93 , //degrees C adc_code = 115
	93 , //degrees C adc_code = 116
	93 , //degrees C adc_code = 117
	92 , //degrees C adc_code = 118
	92 , //degrees C adc_code = 119
	92 , //degrees C adc_code = 120
	91 , //degrees C adc_code = 121
	91 , //degrees C adc_code = 122
	91 , //degrees C adc_code = 123
	90 , //degrees C adc_code = 124
	90 , //degrees C adc_code = 125
	90 , //degrees C adc_code = 126
	89 , //degrees C adc_code = 127
	89 , //degrees C adc_code = 128
	89 , //degrees C adc_code = 129
	88 , //degrees C adc_code = 130
	88 , //degrees C adc_code = 131
	88 , //degrees C adc_code = 132
	87 , //degrees C adc_code = 133
	87 , //degrees C adc_code = 134
	87 , //degrees C adc_code = 135
	86 , //degrees C adc_code = 136
	86 , //degrees C adc_code = 137
	86 , //degrees C adc_code = 138
	86 , //degrees C adc_code = 139
	85 , //degrees C adc_code = 140
	85 , //degrees C adc_code = 141
	85 , //degrees C adc_code = 142
	84 , //degrees C adc_code = 143
	84 , //degrees C adc_code = 144
	84 , //degrees C adc_code = 145
	83 , //degrees C adc_code = 146
	83 , //degrees C adc_code = 147
	83 , //degrees C adc_code = 148
	83 , //degrees C adc_code = 149
	82 , //degrees C adc_code = 150
	82 , //degrees C adc_code = 151
	82 , //degrees C adc_code = 152
	81 , //degrees C adc_code = 153
	81 , //degrees C adc_code = 154
	81 , //degrees C adc_code = 155
	81 , //degrees C adc_code = 156
	80 , //degrees C adc_code = 157
	80 , //degrees C adc_code = 158
	80 , //degrees C adc_code = 159
	80 , //degrees C adc_code = 160
	79 , //degrees C adc_code = 161
	79 , //degrees C adc_code = 162
	79 , //degrees C adc_code = 163
	79 , //degrees C adc_code = 164
	78 , //degrees C adc_code = 165
	78 , //degrees C adc_code = 166
	78 , //degrees C adc_code = 167
	77 , //degrees C adc_code = 168
	77 , //degrees C adc_code = 169
	77 , //degrees C adc_code = 170
	77 , //degrees C adc_code = 171
	76 , //degrees C adc_code = 172
	76 , //degrees C adc_code = 173
	76 , //degrees C adc_code = 174
	76 , //degrees C adc_code = 175
	75 , //degrees C adc_code = 176
	75 , //degrees C adc_code = 177
	75 , //degrees C adc_code = 178
	75 , //degrees C adc_code = 179
	74 , //degrees C adc_code = 180
	74 , //degrees C adc_code = 181
	74 , //degrees C adc_code = 182
	74 , //degrees C adc_code = 183
	73 , //degrees C adc_code = 184
	73 , //degrees C adc_code = 185
	73 , //degrees C adc_code = 186
	73 , //degrees C adc_code = 187
	72 , //degrees C adc_code = 188
	72 , //degrees C adc_code = 189
	72 , //degrees C adc_code = 190
	72 , //degrees C adc_code = 191
	72 , //degrees C adc_code = 192
	71 , //degrees C adc_code = 193
	71 , //degrees C adc_code = 194
	71 , //degrees C adc_code = 195
	71 , //degrees C adc_code = 196
	70 , //degrees C adc_code = 197
	70 , //degrees C adc_code = 198
	70 , //degrees C adc_code = 199
	70 , //degrees C adc_code = 200
	69 , //degrees C adc_code = 201
	69 , //degrees C adc_code = 202
	69 , //degrees C adc_code = 203
	69 , //degrees C adc_code = 204
	69 , //degrees C adc_code = 205
	68 , //degrees C adc_code = 206
	68 , //degrees C adc_code = 207
	68 , //degrees C adc_code = 208
	68 , //degrees C adc_code = 209
	67 , //degrees C adc_code = 210
	67 , //degrees C adc_code = 211
	67 , //degrees C adc_code = 212
	67 , //degrees C adc_code = 213
	66 , //degrees C adc_code = 214
	66 , //degrees C adc_code = 215
	66 , //degrees C adc_code = 216
	66 , //degrees C adc_code = 217
	66 , //degrees C adc_code = 218
	65 , //degrees C adc_code = 219
	65 , //degrees C adc_code = 220
	65 , //degrees C adc_code = 221
	65 , //degrees C adc_code = 222
	64 , //degrees C adc_code = 223
	64 , //degrees C adc_code = 224
	64 , //degrees C adc_code = 225
	64 , //degrees C adc_code = 226
	64 , //degrees C adc_code = 227
	63 , //degrees C adc_code = 228
	63 , //degrees C adc_code = 229
	63 , //degrees C adc_code = 230
	63 , //degrees C adc_code = 231
	63 , //degrees C adc_code = 232
	62 , //degrees C adc_code = 233
	62 , //degrees C adc_code = 234
	62 , //degrees C adc_code = 235
	62 , //degrees C adc_code = 236
	61 , //degrees C adc_code = 237
	61 , //degrees C adc_code = 238
	61 , //degrees C adc_code = 239
	61 , //degrees C adc_code = 240
	61 , //degrees C adc_code = 241
	60 , //degrees C adc_code = 242
	60 , //degrees C adc_code = 243
	60 , //degrees C adc_code = 244
	60 , //degrees C adc_code = 245
	60 , //degrees C adc_code = 246
	59 , //degrees C adc_code = 247
	59 , //degrees C adc_code = 248
	59 , //degrees C adc_code = 249
	59 , //degrees C adc_code = 250
	59 , //degrees C adc_code = 251
	58 , //degrees C adc_code = 252
	58 , //degrees C adc_code = 253
	58 , //degrees C adc_code = 254
	58 , //degrees C adc_code = 255
	57 , //degrees C adc_code = 256
	57 , //degrees C adc_code = 257
	57 , //degrees C adc_code = 258
	57 , //degrees C adc_code = 259
	57 , //degrees C adc_code = 260
	56 , //degrees C adc_code = 261
	56 , //degrees C adc_code = 262
	56 , //degrees C adc_code = 263
	56 , //degrees C adc_code = 264
	56 , //degrees C adc_code = 265
	55 , //degrees C adc_code = 266
	55 , //degrees C adc_code = 267
	55 , //degrees C adc_code = 268
	55 , //degrees C adc_code = 269
	55 , //degrees C adc_code = 270
	54 , //degrees C adc_code = 271
	54 , //degrees C adc_code = 272
	54 , //degrees C adc_code = 273
	54 , //degrees C adc_code = 274
	54 , //degrees C adc_code = 275
	53 , //degrees C adc_code = 276
	53 , //degrees C adc_code = 277
	53 , //degrees C adc_code = 278
	53 , //degrees C adc_code = 279
	53 , //degrees C adc_code = 280
	52 , //degrees C adc_code = 281
	52 , //degrees C adc_code = 282
	52 , //degrees C adc_code = 283
	52 , //degrees C adc_code = 284
	52 , //degrees C adc_code = 285
	51 , //degrees C adc_code = 286
	51 , //degrees C adc_code = 287
	51 , //degrees C adc_code = 288
	51 , //degrees C adc_code = 289
	51 , //degrees C adc_code = 290
	50 , //degrees C adc_code = 291
	50 , //degrees C adc_code = 292
	50 , //degrees C adc_code = 293
	50 , //degrees C adc_code = 294
	49 , //degrees C adc_code = 295
	49 , //degrees C adc_code = 296
	49 , //degrees C adc_code = 297
	49 , //degrees C adc_code = 298
	49 , //degrees C adc_code = 299
	48 , //degrees C adc_code = 300
	48 , //degrees C adc_code = 301
	48 , //degrees C adc_code = 302
	48 , //degrees C adc_code = 303
	48 , //degrees C adc_code = 304
	47 , //degrees C adc_code = 305
	47 , //degrees C adc_code = 306
	47 , //degrees C adc_code = 307
	47 , //degrees C adc_code = 308
	47 , //degrees C adc_code = 309
	46 , //degrees C adc_code = 310
	46 , //degrees C adc_code = 311
	46 , //degrees C adc_code = 312
	46 , //degrees C adc_code = 313
	46 , //degrees C adc_code = 314
	45 , //degrees C adc_code = 315
	45 , //degrees C adc_code = 316
	45 , //degrees C adc_code = 317
	45 , //degrees C adc_code = 318
	45 , //degrees C adc_code = 319
	44 , //degrees C adc_code = 320
	44 , //degrees C adc_code = 321
	44 , //degrees C adc_code = 322
	44 , //degrees C adc_code = 323
	44 , //degrees C adc_code = 324
	43 , //degrees C adc_code = 325
	43 , //degrees C adc_code = 326
	43 , //degrees C adc_code = 327
	43 , //degrees C adc_code = 328
	42 , //degrees C adc_code = 329
	42 , //degrees C adc_code = 330
	42 , //degrees C adc_code = 331
	42 , //degrees C adc_code = 332
	42 , //degrees C adc_code = 333
	41 , //degrees C adc_code = 334
	41 , //degrees C adc_code = 335
	41 , //degrees C adc_code = 336
	41 , //degrees C adc_code = 337
	41 , //degrees C adc_code = 338
	40 , //degrees C adc_code = 339
	40 , //degrees C adc_code = 340
	40 , //degrees C adc_code = 341
	40 , //degrees C adc_code = 342
	40 , //degrees C adc_code = 343
	39 , //degrees C adc_code = 344
	39 , //degrees C adc_code = 345
	39 , //degrees C adc_code = 346
	39 , //degrees C adc_code = 347
	38 , //degrees C adc_code = 348
	38 , //degrees C adc_code = 349
	38 , //degrees C adc_code = 350
	38 , //degrees C adc_code = 351
	38 , //degrees C adc_code = 352
	37 , //degrees C adc_code = 353
	37 , //degrees C adc_code = 354
	37 , //degrees C adc_code = 355
	37 , //degrees C adc_code = 356
	36 , //degrees C adc_code = 357
	36 , //degrees C adc_code = 358
	36 , //degrees C adc_code = 359
	36 , //degrees C adc_code = 360
	36 , //degrees C adc_code = 361
	35 , //degrees C adc_code = 362
	35 , //degrees C adc_code = 363
	35 , //degrees C adc_code = 364
	35 , //degrees C adc_code = 365
	34 , //degrees C adc_code = 366
	34 , //degrees C adc_code = 367
	34 , //degrees C adc_code = 368
	34 , //degrees C adc_code = 369
	34 , //degrees C adc_code = 370
	33 , //degrees C adc_code = 371
	33 , //degrees C adc_code = 372
	33 , //degrees C adc_code = 373
	33 , //degrees C adc_code = 374
	32 , //degrees C adc_code = 375
	32 , //degrees C adc_code = 376
	32 , //degrees C adc_code = 377
	32 , //degrees C adc_code = 378
	31 , //degrees C adc_code = 379
	31 , //degrees C adc_code = 380
	31 , //degrees C adc_code = 381
	31 , //degrees C adc_code = 382
	31 , //degrees C adc_code = 383
	30 , //degrees C adc_code = 384
	30 , //degrees C adc_code = 385
	30 , //degrees C adc_code = 386
	30 , //degrees C adc_code = 387
	29 , //degrees C adc_code = 388
	29 , //degrees C adc_code = 389
	29 , //degrees C adc_code = 390
	29 , //degrees C adc_code = 391
	28 , //degrees C adc_code = 392
	28 , //degrees C adc_code = 393
	28 , //degrees C adc_code = 394
	28 , //degrees C adc_code = 395
	27 , //degrees C adc_code = 396
	27 , //degrees C adc_code = 397
	27 , //degrees C adc_code = 398
	27 , //degrees C adc_code = 399
	26 , //degrees C adc_code = 400
	26 , //degrees C adc_code = 401
	26 , //degrees C adc_code = 402
	26 , //degrees C adc_code = 403
	25 , //degrees C adc_code = 404
	25 , //degrees C adc_code = 405
	25 , //degrees C adc_code = 406
	24 , //degrees C adc_code = 407
	24 , //degrees C adc_code = 408
	24 , //degrees C adc_code = 409
	24 , //degrees C adc_code = 410
	23 , //degrees C adc_code = 411
	23 , //degrees C adc_code = 412
	23 , //degrees C adc_code = 413
	23 , //degrees C adc_code = 414
	22 , //degrees C adc_code = 415
	22 , //degrees C adc_code = 416
	22 , //degrees C adc_code = 417
	21 , //degrees C adc_code = 418
	21 , //degrees C adc_code = 419
	21 , //degrees C adc_code = 420
	21 , //degrees C adc_code = 421
	20 , //degrees C adc_code = 422
	20 , //degrees C adc_code = 423
	20 , //degrees C adc_code = 424
	19 , //degrees C adc_code = 425
	19 , //degrees C adc_code = 426
	19 , //degrees C adc_code = 427
	18 , //degrees C adc_code = 428
	18 , //degrees C adc_code = 429
	18 , //degrees C adc_code = 430
	18 , //degrees C adc_code = 431
	17 , //degrees C adc_code = 432
	17 , //degrees C adc_code = 433
	17 , //degrees C adc_code = 434
	16 , //degrees C adc_code = 435
	16 , //degrees C adc_code = 436
	16 , //degrees C adc_code = 437
	15 , //degrees C adc_code = 438
	15 , //degrees C adc_code = 439
	15 , //degrees C adc_code = 440
	14 , //degrees C adc_code = 441
	14 , //degrees C adc_code = 442
	14 , //degrees C adc_code = 443
	13 , //degrees C adc_code = 444
	13 , //degrees C adc_code = 445
	12 , //degrees C adc_code = 446
	12 , //degrees C adc_code = 447
	12 , //degrees C adc_code = 448
	11 , //degrees C adc_code = 449
	11 , //degrees C adc_code = 450
	11 , //degrees C adc_code = 451
	10 , //degrees C adc_code = 452
	10 , //degrees C adc_code = 453
	 9 , //degrees C adc_code = 454
	 9 , //degrees C adc_code = 455
	 9 , //degrees C adc_code = 456
	 8 , //degrees C adc_code = 457
	 8 , //degrees C adc_code = 458
	 7 , //degrees C adc_code = 459
	 7 , //degrees C adc_code = 460
	 6 , //degrees C adc_code = 461
	 6 , //degrees C adc_code = 462
	 6 , //degrees C adc_code = 463
	 5 , //degrees C adc_code = 464
	 5 , //degrees C adc_code = 465
	 4 , //degrees C adc_code = 466
	 4 , //degrees C adc_code = 467
	 3 , //degrees C adc_code = 468
	 3 , //degrees C adc_code = 469
	 2 , //degrees C adc_code = 470
	 2 , //degrees C adc_code = 471
	 1 , //degrees C adc_code = 472
	 1 , //degrees C adc_code = 473
	 0 , //degrees C adc_code = 474
	-1 , //degrees C adc_code = 475
	-1 , //degrees C adc_code = 476
	-2 , //degrees C adc_code = 477
	-2 , //degrees C adc_code = 478
	-3 , //degrees C adc_code = 479
	-4 , //degrees C adc_code = 480
	-4 , //degrees C adc_code = 481
	-5 , //degrees C adc_code = 482
	-6 , //degrees C adc_code = 483
	-6 , //degrees C adc_code = 484
	-7 , //degrees C adc_code = 485
	-8 , //degrees C adc_code = 486
	-9 , //degrees C adc_code = 487
	-9 , //degrees C adc_code = 488
	-10 , //degrees C adc_code = 489
	-11 , //degrees C adc_code = 490
	-12 , //degrees C adc_code = 491
	-13 , //degrees C adc_code = 492
	-14 , //degrees C adc_code = 493
	-15 , //degrees C adc_code = 494
	-16 , //degrees C adc_code = 495
	-17 , //degrees C adc_code = 496
	-18 , //degrees C adc_code = 497
	-20 , //degrees C adc_code = 498
	-21 , //degrees C adc_code = 499
	-23 , //degrees C adc_code = 500
	-24 , //degrees C adc_code = 501
	-26 , //degrees C adc_code = 502
	-28 , //degrees C adc_code = 503
	-30 , //degrees C adc_code = 504
	-32 , //degrees C adc_code = 505
	-35 , //degrees C adc_code = 506
	-39 , //degrees C adc_code = 507
	-40 , //degrees C adc_code = 508
	-40 , //degrees C adc_code = 509
	-40 , //degrees C adc_code = 510
	-40  //degrees C adc_code = 511
};




#endif /* COMMON_MODULES_TELEMETRY_NTC_TEMPERATURE_LUT_H_ */
