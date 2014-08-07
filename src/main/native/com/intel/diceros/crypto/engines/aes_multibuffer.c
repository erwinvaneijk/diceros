/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <openssl/evp.h>
#include <openssl/err.h>
#include <cpuid.h>
#include <dlfcn.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "aes_multibuffer.h"
#include "config.h"

#define BLOCKSIZE 16

void cleanDLError() {
  dlerror();
}

static EncryptX8 encrypt(void* handle, int keyLength)
{
  if (NULL == handle) {
    return NULL;
  }

  static EncryptX8 X8128 = NULL;
  static EncryptX8 X8192 = NULL;
  static EncryptX8 X8256 = NULL;

  EncryptX8 result = NULL;
  char* funcName = NULL;

  switch (keyLength) {
  case 16:
    funcName = "aes_cbc_enc_128_x8";
    if (NULL == X8128) {
      X8128 = dlsym(handle, funcName);
    }
    result = X8128;
    break;
  case 24:
    funcName = "aes_cbc_enc_192_x8";
    if (NULL == X8192) {
      X8192 = dlsym(handle, funcName);
    }
    result = X8192;
    break;
  case 32:
    funcName = "aes_cbc_enc_256_x8";
    if (NULL == X8256) {
      X8256 = dlsym(handle, funcName);
    }
    result = X8256;
    break;
  default:
    result = NULL;
    break;
  }

  if (NULL == result) {
    DTRACE("invalid key length %d or symbol %s\n", keyLength, funcName);
  }

  return result;
}

static DecryptX1 decrypt(void* handle, int keyLength)
{
  if (NULL == handle) {
    return NULL;
  }

  static DecryptX1 X1128 = NULL;
  static DecryptX1 X1192 = NULL;
  static DecryptX1 X1256 = NULL;


  DecryptX1 result = NULL;
  char* funcName = NULL;

  switch (keyLength) {
  case 16:
    funcName = "iDec128_CBC_by8";
    if (NULL == X1128) {
      X1128 = dlsym(handle, funcName);
    }
    result = X1128;
    break;
  case 24:
    funcName = "iDec192_CBC_by8";
    if (NULL == X1192) {
      X1192 = dlsym(handle, funcName);
    }
    result = X1192;
    break;
  case 32:
    funcName = "iDec256_CBC_by8";
    if (NULL == X1256) {
      X1256 = dlsym(handle, funcName);
    }
    result = X1256;
    break;
  default:
    result = NULL;
    break;
  }

  if (NULL == result) {
    DTRACE("invalid key length %d or symbol %s\n", keyLength, funcName);
  }
  return result ;
}

// mode: 1 for encrypt, 0 for decrypt
static KeySched keyexp(void* handle, int keyLength, int mode) {
  if (NULL == handle) {
    return NULL;
  }

  static KeySched EncKeyExp128 = NULL;
  static KeySched EncKeyExp192 = NULL;
  static KeySched EncKeyExp256 = NULL;

  static KeySched DecKeyExp128 = NULL;
  static KeySched DecKeyExp192 = NULL;
  static KeySched DecKeyExp256 = NULL;

  KeySched result = NULL;
  char* funcName = NULL;

  switch (keyLength + mode) {
  case 16:
    funcName = "aes_keyexp_128_dec";
    if (NULL == DecKeyExp128) {
      DecKeyExp128 = dlsym(handle, funcName);
    }
    result = DecKeyExp128;
    break;
  case 24:
    funcName = "aes_keyexp_192_dec";
    if (NULL == DecKeyExp192) {
      DecKeyExp192 = dlsym(handle, funcName);
    }
    result = DecKeyExp192;
    break;
  case 32:
    funcName = "aes_keyexp_256_dec";
    if (NULL == DecKeyExp256) {
      DecKeyExp256 = dlsym(handle, funcName);
    }
    result = DecKeyExp256;
    break;
  case 16 + 1:
    funcName = "aes_keyexp_128_enc";
    if (NULL == EncKeyExp128) {
      EncKeyExp128 = dlsym(handle, funcName);
    }
    result = EncKeyExp128;
    break;
  case 24 + 1:
    funcName = "aes_keyexp_192_enc";
    if (NULL == EncKeyExp192) {
      EncKeyExp192 = dlsym(handle, funcName);
    }
    result = EncKeyExp192;
    break;
  case 32 + 1:
    funcName = "aes_keyexp_256_enc";
    if (NULL == EncKeyExp256) {
      EncKeyExp256 = dlsym(handle, funcName);
    }
    result = EncKeyExp256;
    break;
  default:
    result = NULL;
    break;
  }

  if (NULL == result) {
    DTRACE("invalid key length %d or symbol %s\n", keyLength, funcName);
  }
  return result ;
}

int aesni_supported() {
  int a, b, c, d;
  __cpuid(1, a, b, c, d);

  return (c >> 25) & 1;
}

int aesmb_keyexp(CipherContext* ctx) {
  if (NULL == ctx || NULL == ctx->aesmbCtx || NULL == ctx->aesmbCtx->handle) {
    DTRACE("Invalid parameter: ctx or key or iv is NULL!");
    return -1;
  }

  void* handle = ctx->aesmbCtx->handle;

  KeySched keySchedFunc = NULL;
  // init encryption key expension
  keySchedFunc = keyexp(handle, ctx->keyLength, 1);
  if (NULL == keySchedFunc) {
    DTRACE("Invalid parameter: key length(%d) is not supported", keyLength);
    return -2;
  }
  keySchedFunc(ctx->key, ctx->aesmbCtx->encryptKeysched);
  // init decryption key expension
  keySchedFunc = keyexp(handle, ctx->keyLength, 0);
  if (NULL == keySchedFunc) {
    DTRACE("Invalid parameter: key length(%d) is not supported", keyLength);
    return -2;
  }
  keySchedFunc(ctx->key, ctx->aesmbCtx->decryptKeysched);

  return 0;
}

int aesmb_keyinit(CipherContext* ctx, uint8_t* key, int keyLength) {
  if (NULL == key || NULL == ctx) {
    return 0;
  }

  if (keyLength != ctx->keyLength) {
    free(ctx->key);
    ctx->keyLength = keyLength;
    ctx->key = (uint8_t*) malloc (keyLength * sizeof(uint8_t));

    ctx->aesmbCtx->efunc = encrypt(ctx->aesmbCtx->handle, keyLength);
    ctx->aesmbCtx->dfunc = decrypt(ctx->aesmbCtx->handle, keyLength);
  }

  memcpy(ctx->key, key, keyLength);

  if (NULL == ctx->aesmbCtx->efunc || NULL == ctx->aesmbCtx->dfunc) {
    DTRACE("Invalid parameter: key length(%d) is not supported", keyLength);
    return -3;
  }

  if (!aesni_supported()) {
    return -4;
  }

  return aesmb_keyexp(ctx);
}

int aesmb_ivinit(CipherContext* ctx, uint8_t* iv, int ivLength) {
  if (NULL == iv || NULL == ctx) {
    return 0;
  }

  if (ivLength != ctx->ivLength) {
    free(ctx->iv);
    ctx->ivLength = ivLength;
    ctx->iv = (uint8_t*) malloc (ivLength * sizeof(uint8_t) * PARALLEL_LEVEL);
  }

  int i,j = 0;
  for (i = 0 ; i < PARALLEL_LEVEL ; i++) {
    memcpy(ctx->iv + i * ivLength, iv, ivLength);
    // generate seven different IVs
    for(j=0 ;j <16 ;j++){
      *(ctx->iv + i * ivLength + j) = *(ctx->iv + i * ivLength + j) +1;
    }
  }

  return ivLength - BLOCKSIZE;
}

int aesmb_keyivinit(CipherContext* ctx, uint8_t* key, int keyLength, uint8_t* iv, int ivLength) {
  int result1 = aesmb_keyinit(ctx, key, keyLength);
  int result2 = aesmb_ivinit(ctx, iv, ivLength);

  if (result1 || result2) {
    return -1;
  }

  return 0;
}

int aesmb_ctxinit(CipherContext* ctx,
              void* handle,
              uint8_t* key,
              uint8_t  keyLength,
              uint8_t* iv,
              uint8_t  ivLength) {
  // Do not check handle, since key and iv will need to be stored in context,
  // even handle is NULL
  if (NULL == ctx || NULL == key || NULL == iv) {
    DTRACE("Invalid parameter: ctx or key or iv is NULL!");
    return -1;
  }

  if (ivLength != BLOCKSIZE) {
    DTRACE("Invalid parameter: iv length is not 128bit!");
    return -2;
  }

  ctx->aesmbCtx->handle = handle;
  return aesmb_keyivinit(ctx, key, keyLength, iv, ivLength);
}

int aesmb_streamlength(int inputLength) {
  int mbUnit = PARALLEL_LEVEL * BLOCKSIZE;
  int mbBlocks = inputLength / mbUnit;
  return BLOCKSIZE * mbBlocks;
}

int aesmb_encrypt(CipherContext* ctx,
              uint8_t* input,
              int inputLength,
              uint8_t* output,
              int* outputLength
              )
{
  if (NULL == ctx || NULL == input || NULL == output || inputLength < 0 ) {
    DTRACE("Invalid parameter: ctx or input or output is NULL!");
    return -1;
  }

  int mbUnit = PARALLEL_LEVEL * BLOCKSIZE;
  int mbBlocks = inputLength / mbUnit;
  int mbTotal = inputLength - inputLength % mbUnit;
  *outputLength = mbTotal;

  if (mbBlocks == 0) {
    return *outputLength;
  }

  sAesData_x8 data;
  data.keysched = ctx->aesmbCtx->encryptKeysched;
  data.numblocks = mbBlocks;

  // init iv
  uint8_t iv[PARALLEL_LEVEL*BLOCKSIZE];
  memcpy(iv, ctx->iv, PARALLEL_LEVEL*BLOCKSIZE);

  int i;
  for (i =0; i < PARALLEL_LEVEL; i++) {
    int step = i * BLOCKSIZE * mbBlocks;
    data.inbuf[i] = input + step;
    data.outbuf[i] = output + step;
    data.iv[i] = iv + i*BLOCKSIZE;
  }

  (ctx->aesmbCtx->efunc) (&data); // encrypt in parallel

  return *outputLength;
}

int aesmb_decrypt(CipherContext* ctx,
              uint8_t* input,
              int inputLength,
              uint8_t* output,
              int* outputLength
              )
{
  if (NULL == ctx || NULL == input || NULL == output || inputLength < 0 ) {
    DTRACE("Invalid parameter: ctx or input or output is NULL!");
    return -1;
  }

  int mbUnit = BLOCKSIZE * PARALLEL_LEVEL;
  int mbBlocks = inputLength / mbUnit;
  int mbTotal = inputLength - inputLength % mbUnit;
  *outputLength = mbTotal;

  if (mbBlocks == 0) {
    return *outputLength;
  }

  sAesData data;
  data.keysched = ctx->aesmbCtx->decryptKeysched;
  data.numblocks = mbBlocks;

  // init iv
  uint8_t iv[PARALLEL_LEVEL*BLOCKSIZE];
  memcpy(iv, ctx->iv, PARALLEL_LEVEL*BLOCKSIZE);

  int i;
  for (i =0; i < PARALLEL_LEVEL; i++) {
    int step = i * BLOCKSIZE * mbBlocks;
    data.inbuf = input + step;
    data.outbuf = output + step;
    data.iv = iv + i*BLOCKSIZE;
    (ctx->aesmbCtx->dfunc)(&data); // decrypt by each stream
  }

  return *outputLength;
}

CipherContext* createCipherContextMB(void* handle, signed char* key, int keylen, signed char* iv, int ivlen) {
  CipherContext* ctx = (CipherContext*) malloc(sizeof(CipherContext));
  memset(ctx, 0, sizeof(CipherContext));

  ctx->opensslCtx = (EVP_CIPHER_CTX*) malloc(sizeof(EVP_CIPHER_CTX));

  ctx->aesmbCtx = (sAesContext*) malloc(sizeof(sAesContext));
  memset(ctx->aesmbCtx, 0, sizeof(sAesContext));

  // init iv and key
  int result = aesmb_ctxinit(ctx, handle, (uint8_t*)key, keylen, (uint8_t*)iv, ivlen);

  ctx->aesmbCtx->aesEnabled = aesni_supported() && result == 0;
  return ctx;
}

long init(int forEncryption, signed char* nativeKey, int keyLength, signed char* nativeIv,
    int ivLength, int padding , long oldContext, int* loadLibraryResult) {
  // Load libcrypto.so, if error, throw java exception
  if (!loadLibrary(HADOOP_CRYPTO_LIBRARY)) {
    *loadLibraryResult = -1;
    return 0;
  }

  // load libaesmb.so, if error, print debug message
  void* handle = loadLibrary(HADOOP_AESMB_LIBRARY);
  if (NULL == handle) {
    *loadLibraryResult = -2;
  }

  // cleanup error
  cleanDLError();

  if (oldContext != NULL) {
    destroyCipherContext((CipherContext*)oldContext);
  }

  // init all context
  CipherContext* ctx = createCipherContextMB(handle, nativeKey, keyLength, nativeIv, ivLength);
  // init openssl context, by using localized key & iv
  EVP_CIPHER_CTX_init(ctx->opensslCtx);
  opensslResetContext(forEncryption, ctx->opensslCtx, ctx);
  if (PADDING_NOPADDING == padding) {
    EVP_CIPHER_CTX_set_padding(ctx->opensslCtx, 0);
  } else if (PADDING_PKCS5PADDING == padding){
    EVP_CIPHER_CTX_set_padding(ctx->opensslCtx, 1);
  }
  return (long)ctx;
}

void opensslResetContext(int forEncryption, EVP_CIPHER_CTX* context, CipherContext* cipherContext) {
  opensslResetContextMB(forEncryption, context, cipherContext, 0);
}

void opensslResetContextMB(int forEncryption, EVP_CIPHER_CTX* context,
    CipherContext* cipherContext, int count) {
  int keyLength = cipherContext->keyLength;
  unsigned char* nativeKey = (unsigned char*) cipherContext->key;
  unsigned char* nativeIv = (unsigned char*) cipherContext->iv + count * 16;

  cryptInit cryptInitFunc = getCryptInitFunc(forEncryption);
  if (keyLength == 32) {
    cryptInitFunc(context, EVP_aes_256_cbc(), NULL,
        (unsigned char *) nativeKey, (unsigned char *) nativeIv);
  } else if (keyLength == 24) {
    cryptInitFunc(context, EVP_aes_192_cbc(), NULL,
        (unsigned char *) nativeKey, (unsigned char *) nativeIv);
  } else if (keyLength == 16) {
    cryptInitFunc(context, EVP_aes_128_cbc(), NULL,
        (unsigned char *) nativeKey, (unsigned char *) nativeIv);
  }
}

void reset(CipherContext* cipherContext, uint8_t* nativeKey, uint8_t* nativeIv) {
    // reinit openssl context by localized key&iv
    aesmb_keyivinit(cipherContext, nativeKey, cipherContext->keyLength, (uint8_t*)nativeIv, cipherContext->ivLength);

    EVP_CIPHER_CTX * ctx = (EVP_CIPHER_CTX *)(cipherContext->opensslCtx);
    opensslResetContext(ctx->encrypt, ctx, cipherContext);
}

int opensslEncrypt(EVP_CIPHER_CTX* ctx, unsigned char* output, int* outLength, unsigned char* input, int inLength) {
  int outLengthFinal;
  *outLength = 0;

  if(inLength && !EVP_EncryptUpdate(ctx, output, outLength, input, inLength)){
    printf("ERROR in EVP_EncryptUpdate \n");
    ERR_print_errors_fp(stderr);
    return -1;
  }

  if(!EVP_EncryptFinal_ex(ctx, output + *outLength, &outLengthFinal)){
    printf("ERROR in EVP_EncryptFinal_ex \n");
    ERR_print_errors_fp(stderr);
    return -1;
  }

  *outLength = *outLength + outLengthFinal;
  return 0;
}

int opensslDecrypt(EVP_CIPHER_CTX* ctx, unsigned char* output, int* outLength, unsigned char* input, int inLength) {
  int outLengthFinal;
  *outLength = 0;

  if(inLength && !EVP_DecryptUpdate(ctx, output, outLength, input, inLength)){
    printf("ERROR in EVP_DecryptUpdate\n");
    ERR_print_errors_fp(stderr);
    return -1;
  }

  if(!EVP_DecryptFinal_ex(ctx, output + *outLength, &outLengthFinal)){
    printf("ERROR in EVP_DecryptFinal_ex\n");
    ERR_print_errors_fp(stderr);
    return -1;
  }

  *outLength = *outLength + outLengthFinal;
  return 0;
}

int bufferCrypt(CipherContext* cipherContext, const char* input, int inputLength, char* output) {
  EVP_CIPHER_CTX * ctx = (EVP_CIPHER_CTX *)cipherContext->opensslCtx;
  sAesContext* aesCtx = (sAesContext*) cipherContext->aesmbCtx;
  int aesEnabled = aesCtx->aesEnabled;
  int aesmbApplied = 0; // 0 for not applied

  int outLength = 0;
  int outLengthFinal = 0;

  unsigned char * header = NULL;
  int extraOutputLength = 0;
  if (ctx->encrypt == ENCRYPTION) {
    header = output;
    output = header + HEADER_LENGTH;
    extraOutputLength = HEADER_LENGTH;
  } else {
    header = input;
    input = header + HEADER_LENGTH;
    inputLength -= HEADER_LENGTH;
  }

  if (ctx->encrypt == ENCRYPTION) {
    if (aesEnabled) {
      // try to apply multi-buffer optimization
      int encrypted = aesmb_encrypt(cipherContext, input, inputLength, output, &outLength);
      if (encrypted < 0) {
      // reportError(env, "AES multi-buffer encryption failed.");
        return 0;
      }
      aesmbApplied = encrypted;
      input += encrypted;
      inputLength -=encrypted;
      output +=outLength; // rest of data will use openssl to perform encryption
    }

    // encrypt with padding
    EVP_CIPHER_CTX_set_padding(ctx, 1);
    // encrypt the rest
    opensslEncrypt(ctx, output, &outLengthFinal, input, inputLength);

    if (aesmbApplied) {
      header[0] = 1; // enabled
      header[1] = outLengthFinal - inputLength; // padding
    } else {
      header[0] = 0;
      header[1] = 0;
    }
  } else {
    // read custom header
    if (header[0]) {
      int padding = (int) header[1];
      if (aesEnabled) {
        int decrypted = aesmb_decrypt(cipherContext, input, inputLength - padding, output, &outLengthFinal);
        if (decrypted < 0) {
        // todo?
        // reportError(env, "Data can not be decrypted correctly");
          return 0;
        }

        input += outLengthFinal;
        inputLength -= outLengthFinal;
        output += outLengthFinal;
        outLength += outLengthFinal;
      } else {
        int step = aesmb_streamlength(inputLength - padding);
        outLength = 0;
        int i;
        for (i = 0; i < PARALLEL_LEVEL; i++) {
          //reset open ssl context
          opensslResetContextMB(ctx->encrypt, ctx, cipherContext, i);
          //clear padding, since multi-buffer AES did not have padding
          EVP_CIPHER_CTX_set_padding(ctx, 0);
          //decrypt using open ssl
          opensslDecrypt(ctx, output, &outLengthFinal, input, step);

          input += step;
          inputLength -= step;
          output += outLengthFinal;
          outLength += outLengthFinal;
        }
      }
    }

    //reset open ssl context
    opensslResetContext(ctx->encrypt, ctx, cipherContext);
    //enable padding, the last buffer need padding
    EVP_CIPHER_CTX_set_padding(ctx, 1);
    //decrypt using open ssl
    opensslDecrypt(ctx, output, &outLengthFinal, input, inputLength);
  }

  return outLength + outLengthFinal + extraOutputLength;
}
