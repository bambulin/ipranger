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
import sys
import string
import random
import ipaddress
from tqdm import tqdm
from itertools import product

MAX_IPV4 = ipaddress.IPv4Address._ALL_ONES  # 2 ** 32 - 1
MAX_IPV6 = ipaddress.IPv6Address._ALL_ONES  # 2 ** 128 - 1
#source: https://stackoverflow.com/questions/21014618/python-randomly-generated-ip-address-as-string

# Arguments:
# letters: number of letters that the user id will contain, # numbers: respective numbers for the user id
# n: iterations of swaps of IP addresses, dataset: the output dataset file, testfile: the output test file
# version: 4 for IPv4 and 6 for IPv6 
def test1(letters, numbers,n,dataset,testfile,version):

	# set that holds the generated IP addresses
	ips = set ()
	# list that holds the generated user IDs
	ids = []

	# open two files for writing: input dataset and test file
	with open(dataset,"w") as d,open(testfile,"w") as t:
		print("\n[+] Generating user IDs") 
		# generate all the possible combinations of #letters with a vocabulary of ascii lowercase charactes
		for letter in product(string.ascii_lowercase, repeat=letters):
			x = ''.join(letter)

			# for each combination, append all the possible numbers
			# the final ID will have the form: (letters){pow(26,letters)}-(numbers){pow(10,numbers)}
			for i in range(0,pow(10,numbers)):
				ids.append(x+"-"+str(i))

		# for every user generate an IP address
		print("[+] Generating initial matching") 
		for i in tqdm(range(len(ids))):
			# generate unique IP addresses in order to avoid collisions (same IP address to N different users)
			while True: 
				# if version equals 4, generate an IPv4 address
				if version == 4:				
					ip = str(ipaddress.IPv4Address._string_from_ip_int(random.randint(0, MAX_IPV4))) + "/32"
				# if version equals 6, generate an IPv6 address
				elif version == 6:
					ip = str(ipaddress.IPv6Address._string_from_ip_int(random.randint(0, MAX_IPV6))) + "/128"

				# store the current size of the set
				old_len = len(ips)
				# update the set 
				ips.add(ip)
				# if the new size is larger, then this is a unique IP address, else generate new one
				if len(ips)>old_len:
					break
			d.write(ids[i]+"\n"+ip+"\n")

		ips_list = list(ips) 

		print("[+] Shuffling...") 
		for rounds in range(n):
			# shuffle the list
			random.shuffle(ips_list)
			sys.stdout.write('\r')
			sys.stdout.write("Iteration #%d " %(rounds))
			sys.stdout.flush()


			for i in range(len(ids)):

				# write tuple to the dataset
				d.write(ids[i]+"\n"+ips_list[i]+"\n")

				# if this is the last round, create the test file that holds the last know address of each user
				if (rounds == n-1):
					t.write(ips_list[i]+"\n"+ids[i]+"\n")




if __name__ == "__main__":

	#test1(1,0,1,"dataset1v4_small.txt","test1v4_small.txt",4)
	# Small Dataset
	## The total number of combinations is pow(26,<num_of_letters>:2)*pow(10,<num_of_numbers>:1) = 6760	
	## There are created 74360 touples of <user,address> as there are occuring 10 swaps and 1 initial assignment = 6760*(10+1)
	test1(2,1,10,"dataset1v4_small.txt","test1v4_small.txt",4)
	test1(2,1,10,"dataset1v6_small.txt","test1v6_small.txt",6)

	# Large Dataset
	## The total number of combinations is pow(26,<num_of_letters>:2)*pow(10,<num_of_numbers>:3) = 676000	
	## There are created 6827600 touples of <user,address> as there are occuring 100 swaps and 1 initial assignment = 6760 *(100+1)
	test1(2,3,100,"dataset1v4_large.txt","test1v4_large.txt",4)
	test1(2,3,100,"dataset1v6_large.txt","test1v6_large.txt",6)