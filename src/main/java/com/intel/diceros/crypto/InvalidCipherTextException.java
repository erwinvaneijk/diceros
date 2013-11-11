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

package com.intel.diceros.crypto;

/**
 * this exception is thrown whenever we find something we don't expect in a
 * message.
 */
public class InvalidCipherTextException extends CryptoException {
  private static final long serialVersionUID = -312034505940387201L;

  public InvalidCipherTextException() {
  }

  /**
   * create a InvalidCipherTextException with the given message.
   *
   * @param message the message to be carried with the exception.
   */
  public InvalidCipherTextException(String message) {
    super(message);
  }

  /**
   * create a InvalidCipherTextException with the given message.
   *
   * @param message the message to be carried with the exception.
   * @param cause   the root cause of the exception.
   */
  public InvalidCipherTextException(String message, Throwable cause) {
    super(message, cause);
  }
}
