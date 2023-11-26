
import sys
import os


if __name__ == "__main__":
	if len(sys.argv) != 2:
		print(f'usage: python script.py <directory_path>')
		sys.exit(1)

	directory_path: str = sys.argv[1]
