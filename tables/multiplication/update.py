import os

files = [fi for fi in os.listdir() if fi != "update.py"]

for og_name in files:
	new_name = og_name[:-3] + "txt"
	new_lines = []

	with open(og_name, 'r') as f:
		lines = f.readlines()

		for line in lines:
			rm_line = line.replace('None', '-1').replace(',', ' ')
			new_lines.append(rm_line)

	with open(new_name, 'w') as f:
		for new_line in new_lines:
			f.write(new_line)


