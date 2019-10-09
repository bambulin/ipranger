# Copyright (C) 2019 Contributors to ipranger project.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# author: Andronikos Kyriakou

import pprint
import string
import random
import time
import sys
from faker import Faker
from math import log10,ceil
from tqdm import tqdm
from netaddr import *
from itertools import product


def random_str(length):
	return ''.join(random.sample(string.ascii_lowercase,length))

def random_hex(length):
	return ''.join(random.sample('0123456789abcdef',length))

def random_num(length):
	return ''.join('%s'%random.randint(0, 9) for i in range(length))

def random_mask_v4():
	return ''.join('%s'%random.randint(5, 32))

def random_mask_v6():
	return ''.join('%s'%random.randint(10, 128))

## This test aims to create a small volume of users (6760) that are frequently changing address
## Test0 tests only the change of address,so the last address of each user should be the correct.
## The created addresses are unique and have a mask of /32 or /128 so we are not covering the case of subnets.
## We are creating 135200 touples of <user,address> so each user in average changes 10 times address.
def test0(letters, numbers,dataset,testfile):
	faker = Faker()
	ipv4 = []
	ipv6 = []
	tuples = {}
	ids = []
	with open(dataset,"w") as d,open(testfile,"w") as t:

		for letter in product(string.ascii_lowercase, repeat=letters):
			x = ''.join(letter)
			for i in range(0,pow(10,numbers)):
				ids.append(x+"-"+str(i))


		total = 10 * len(ids)

		for i in tqdm(range(total)):
			id_=ids[random.randint(0,len(ids)-1)]
			## the total number of combinations is
			## pow(26,<num_of_letters>:2)*pow(10,<num_of_numbers>:1) = 6760
			if i%2==0:
				while True:
					ip = str(faker.ipv4()) + "/" + "32"
					if ip not in ipv4:
						ipv4.append(ip)
						break
			else:
				while True:
					ip = str(faker.ipv6()) + "/" + "128"
					if ip not in ipv6:
						ipv6.append(ip)
						break
			tuples[id_] = ip
			d.write(id_+"\n"+ip+"\n")
		# Create the test file - it holds the last known address of a user
		for i in tuples:
			t.write(tuples[i]+"\n"+i+"\n")




if __name__ == "__main__":
	test0(2,1,"dataset0.txt","test0.txt")






