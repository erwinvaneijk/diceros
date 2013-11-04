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

package com.intel.diceros.crypto.engines;

import com.intel.diceros.crypto.BlockCipher;
import com.intel.diceros.crypto.DataLengthException;
import com.intel.diceros.crypto.params.CipherParameters;
import com.intel.diceros.crypto.params.KeyParameter;

import java.nio.ByteBuffer;

/**
 * This class implements the <i>BlockCipher</i> interface. It depends on the
 * underlying openssl library to do the actual encryption and decryption work.
 */
public class AESOpensslEngine implements BlockCipher {
  private boolean forEncryption = false;
  private int blockSize = 16;
  private String mode;
  private String padding = "NOPADDING";
  private byte[] IV;
  CipherParameters params = null;
  private long context = 0; // context used by openssl

  public AESOpensslEngine(String mode) {
    this.mode = mode;
  }

  public AESOpensslEngine(String mode, String padding) {
    this.mode = mode;
    this.padding = padding;
  }

  @Override
  public void init(boolean forEncryption, CipherParameters params)
          throws IllegalArgumentException {
    if (params instanceof KeyParameter) {
      this.forEncryption = forEncryption;
      this.params = params;
      context = initWorkingKey(((KeyParameter) params).getKey(), forEncryption,
              mode, padding, IV, context);
    } else {
      throw new IllegalArgumentException(
              "invalid parameter passed to AES init - "
                      + params.getClass().getName());
    }
  }

  @Override
  public String getAlgorithmName() {
    return "AES";
  }

  @Override
  public int getBlockSize() {
    return blockSize;
  }

  @Override
  public int processBlock(byte[] in, int inOff, int inLen, byte[] out,
                          int outOff) throws DataLengthException, IllegalStateException {
    return processBlock(context, in, inOff, inLen, out, outOff);
  }

  @Override
  public int doFinal(byte[] out, int outOff) {
    return doFinal(context, out, outOff);
  }

  @Override
  public void reset() {
    init(forEncryption, params);
  }

  @Override
  public int bufferCrypt(ByteBuffer input, ByteBuffer output, boolean isUpdate) {
    return bufferCrypt(context, input, input.position(), input.limit(), output,
            output.position(), isUpdate);
  }

  private native int getBlockSize(long context);

  private native int bufferCrypt(long context, ByteBuffer input, int inputPos,
                                 int inputLimit, ByteBuffer output, int outputPos, boolean isUpdate);

  private native long initWorkingKey(byte[] key, boolean forEncryption,
                                     String mode, String padding, byte[] IV, long context);

  private native int processBlock(long context, byte[] in, int inOff,
                                  int inLen, byte[] out, int outOff);

  private native int doFinal(long context, byte[] out, int outOff);

  @Override
  public void setIV(byte[] IV) {
    this.IV = IV;
  }

  @Override
  public void setPadding(String padding) {
    this.padding = padding;
  }

  @Override
  public String getMode() {
    return this.mode;
  }

  @Override
  public String getPadding() {
    return this.padding;
  }

  @Override
  public int getHeadLength() {
    return 0;
  }
}