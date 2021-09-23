#!/usr/bin/python3

import time
import requests
from bs4 import BeautifulSoup as bs


def mul_scrape(url, out_path):
	"""
	Description: creates a csv of a multiplication table from url of online calculator (lazy)
	Input: url, url to scrape
				 out_path, where to write csv
	Output: None, writes file to out_path
	"""

	start = time.time()
	data = requests.get(url)
	soup = bs(data.text, "html.parser")
	table = soup.find_all("table")[1]  # table is in 2nd tbody elem on page

	tr_elems = table.find_all("tr")
	row_strings = [' '.join(['0' for _ in range(len(tr_elems))])]

	for i, tr in enumerate(table.find_all("tr")):
		s = "0"
		if i != 0:  # first row are actual values
			for j, td in enumerate(tr.find_all("td")):
				if j != 0:  # first col are actual values
					s += f" {td.text.strip()}"

		row_strings.append(s)
	
	with open(out_path, 'w') as f:
		for row in row_strings:
			f.write(f"{row}\n")

	stop = time.time()
	print(f"done with {out_path} in {stop - start} sec")


def div_scrape(base_url, p, max_num, out_path):
	"""
	Description: generate a csv table for division in the given finite field using online calculator
	Input: base_url, the first part of the url before the division parameters
				 p, number (index?) of the polynomial to pass into url
				 max_num, 2**n (last row/col represents max_num - 1)
				 out_path, where to write csv
	Output: None return, writes a file to out_path
  """

	# generate urls as 2d list with same dims as table
	# output value for each is row idx / col idx (i.e. table[1][2] is 1 / 2)
	urls = [[f"{base_url}{str(numer)}&den={str(denom)}&f=d&p={p}&d=1" for denom in range(1, max_num)] for numer in range(1, max_num)]

	count = 0
	start = time.time()

	with open(out_path, 'a') as fi:
		fi.write(f"-1 {' '.join(['0'] * (max_num - 1))}\n")

		for url_list in urls:
			row = ["-1"]  # div by 0 is the first entry

			for url in url_list:
				if count % 50 == 0 and count != 0:
						time.sleep(5)  # avoid no resource error

				print(f"count={count}, {url}")

				data = requests.get(url)
				soup = bs(data.text, "html.parser")
				ans = soup.find("td", {"class": "tdr"}).text.strip()
				row.append(ans)

				count += 1
				time.sleep(0.5)  # avoid no resource error

				fi.write(f"{' '.join(row)}\n")
				fi.flush()

	stop = time.time()
	print(f"done with {out_path} in {stop - start} sec")


def main():
	# multiplication tables
	mul_base_url = "http://www.ee.unb.ca/cgi-bin/tervo/galois3.pl?p="
	mul_out_dir = "../tables/multiplication"
	urls_dict = {"2to3_1011.csv": f"{mul_base_url}2", "2to4_10011.csv": f"{mul_base_url}4", "2to5_100101.csv": f"{mul_base_url}6", "2to6_1000011.csv": f"{mul_base_url}12"}

	for path, url in urls_dict.items():
		mul_scrape(url, f"{mul_out_dir}/{path}")

	# division tables
	div_base_url = "http://www.ee.unb.ca/cgi-bin/tervo/calc2.pl?num="
	div_out_dir = "../tables/division"

	# key: file path, value[0]: polynomial num for url, value[1]: 2**n
	div_path_dict = {"2to3_1011.csv": ["2", 8], "2to4_10011.csv": ["4", 16], "2to5_100101.csv": ["6", 32], "2to6_1000011.csv": ["12", 64]}
	
	for path, content in div_path_dict.items():
		div_scrape(div_base_url, content[0], content[1], f"{div_out_dir}/{path}")


if __name__ == "__main__":
	main()

