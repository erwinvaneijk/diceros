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

package com.intel.diceros.test.aes;

import com.intel.diceros.provider.DicerosProvider;
import com.sun.org.apache.bcel.internal.generic.CPInstruction;

import java.security.Security;

public class AESCBCTest extends AESAbstarctTest {
  static String[] CBCNoPaddingTests = {
  /*
   * key_len,key,iv,plainText,cipherText
   */
  "128",
  "2b7e151628aed2a6abf7158809cf4f3c",
  "000102030405060708090a0b0c0d0e0f", 
  "6bc1bee22e409f96e93d7e117393172a",
  "7649abac8119b246cee98e9b12e9197d",

  "128",
  "2b7e151628aed2a6abf7158809cf4f3c",
  "7649ABAC8119B246CEE98E9B12E9197D", 
  "ae2d8a571e03ac9c9eb76fac45af8e51",
  "5086cb9b507219ee95db113a917678b2",

  "128", 
  "2b7e151628aed2a6abf7158809cf4f3c",
  "5086CB9B507219EE95DB113A917678B2", 
  "30c81c46a35ce411e5fbc1191a0a52ef",
  "73bed6b8e3c1743b7116e69e22229516",

  "128", 
  "2b7e151628aed2a6abf7158809cf4f3c",
  "73BED6B8E3C1743B7116E69E22229516", 
  "f69f2445df4f9b17ad2b417be66c3710",
  "3ff1caa1681fac09120eca307586e1a7",

  "256",
  "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
  "000102030405060708090a0b0c0d0e0f", 
  "6bc1bee22e409f96e93d7e117393172a",
  "f58c4c04d6e5f1ba779eabfb5f7bfbd6",

  "256",
  "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
  "F58C4C04D6E5F1BA779EABFB5F7BFBD6", 
  "ae2d8a571e03ac9c9eb76fac45af8e51",
  "9cfc4e967edb808d679f777bc6702c7d",

  "256",
  "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
  "9CFC4E967EDB808D679F777BC6702C7D", 
  "30c81c46a35ce411e5fbc1191a0a52ef",
  "39f23369a9d9bacfa530e26304231461",

  "256",
  "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4",
  "39F23369A9D9BACFA530E26304231461", 
  "f69f2445df4f9b17ad2b417be66c3710",
  "b2eb05e2c39be9fcda6c19078c6a9d1b" 
  };
  
  
  static String[] CBCPKCS5PaddingTests = {
  /*
   * key_len,key,iv,plainText,cipherText
   */
  //Test#0 for input of 6 bytes
  "128", 
  "ac5800ac3cb59c7c14f36019e43b44fe",
  "f013ce1ec901b5b60a85a986b3b72eba", 
  "f6cee5ff28fd",
  "e8a846fd9718507371604504d4ca1ac7",

  //Test#0 for input of 7 bytes
  "128",
  "24c4328aeffc0ca354a3215a3da23a38",
  "c43c6269bb8c1dbba3bc22b7ba7e24b1", 
  "76cdfdf52a9753",
  "009e935f3fe4d57b57fc3127a8873d8c",
  
  //Test#0 for input of 8 bytes
  "128", 
  "4035227440a779dbd1ed75c6ae78cef5",
  "8faff161a5ec06e051066a571d1729d9", 
  "b103c928531d8875",
  "b3d8df2c3147b0752a7e6bbbcc9d5758",

  //Test#0 for input of 15 bytes
  "128", 
  "0784fa652e733cb699f250b0df2c4b41",
  "106519760fb3ef97e1ccea073b27122d", 
  "6842455a2992c2e5193056a5524075",
  "56a8e0c3ee3315f913693c0ca781e917",

  //Test#0 for input of 16 bytes
  "128",
  "04952c3fcf497a4d449c41e8730c5d9a",
  "53549bf7d5553b727458c1abaf0ba167", 
  "c9a44f6f75e98ddbca7332167f5c45e3",
  "7fa290322ca7a1a04b61a1147ff20fe66fde58510a1d0289d11c0ddf6f4decfd",
  
  //Test#0 for input of 32 bytes
  "128",
  "2ae7081caebe54909820620a44a60a0f",
  "fc5e783fbe7be12f58b1f025d82ada50", 
  "1ba93ee6f83752df47909585b3f28e56693f89e169d3093eee85175ea3a46cd3",
  "7944957a99e473e2c07eb496a83ec4e55db2fb44ebdd42bb611e0def29b23a73ac37eb0f4f5d86f090f3ddce3980425a",

  //Test#0 for input of 33 bytes
  "128",
  "898be9cc5004ed0fa6e117c9a3099d31",
  "9dea7621945988f96491083849b068df", 
  "0397f4f6820b1f9386f14403be5ac16e50213bd473b4874b9bcbf5f318ee686b1d",
  "e232cd6ef50047801ee681ec30f61d53cfd6b0bca02fd03c1b234baa10ea82ac9dab8b960926433a19ce6dea08677e34"};
  public AESCBCTest() {
    super("AES/CBC/NoPadding", "DC", CBCNoPaddingTests);
  }

  public AESCBCTest(String cipherName, String providerName, String[] cipherTest) {
    super(cipherName, providerName, cipherTest);
  }

  public void testAESCBC() {
    Security.addProvider(new DicerosProvider());
    runTest(new AESCBCTest("AES/CBC/NoPadding", "DC", CBCNoPaddingTests));
    runTest(new AESCBCTest("AES/CBC/PKCS5Padding", "DC", CBCPKCS5PaddingTests));
  }

}
