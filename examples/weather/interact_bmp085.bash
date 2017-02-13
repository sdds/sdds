#!/bin/bash

receive_string_b="received ipc response: [success][4]["
receive_string_e="]"

convert_to_short() {
	printf "
import sys\n
\n
x = int(sys.argv[1])\n
if x > ((1 << 15) - 1):\n
        print - ((1 << 16) - x)\n
else:\n
        print x\n
" | python - "$1"
}

write_control() {
	echo twi_start
	echo twi_transmit_address 0xEE
	echo twi_transmit_data $1
	echo twi_transmit_data $2
	echo twi_stop
	echo quit
}

get_reg() {
	echo twi_start
	echo twi_transmit_address 0xEE
	echo twi_transmit_data $1
	echo twi_start
	echo twi_receive_address 0xEE

	for i in $(seq 1 "$2" ) ; do
		echo twi_receive_data
		echo twi_receive_data
	done

	echo twi_stop
	echo quit
}

convert_to_word() {
	prev_byte=

	while read line ; do
		case "$line" in
			"$receive_string_b"*"$receive_string_e")
				work=${line#"${receive_string_b}"}
				work=${work%*"${receive_string_e}"}

				if [ -z "$prev_byte" ] ; then
					prev_byte=$work
				else
					word=$(python -c "print ($prev_byte << 8) | $work")
					prev_byte=
					echo "${receive_string_b}${word}${receive_string_e}"
				fi
				;;
			*)
				echo "$line"
				;;
		esac
	done
}

read_word() {
	start="$1"
	count="$2"

	get_reg "$start" "$count" | ./test-write | convert_to_word | while read line ; do
		case "$line" in
			"$receive_string_b"*"$receive_string_e")
				work=${line#"${receive_string_b}"}
				work=${work%*"${receive_string_e}"}
				echo "$work"
				;;
			*)
				;;
		esac
	done
		
}

calculate_temp() {
	# uses last read eeprom
	printf "
import sys\n
ac1=8648\n
ac2=-1321\n
ac3=-14650\n
ac4=33981\n
ac5=24547\n
ac6=20262\n
b1=5498\n
b2=73\n
mb=-32768\n
mc=-11075\n
md=2432\n
\n
x1 = (int(sys.argv[1]) - ac6) * ac5 / (2 ** 15)\n
x2 = mc * 2 ** 11 / (x1 + md)\n
b5 = x1 + x2\n
\n
#print 'x1: %%d' %% x1\n
#print 'x2: %%d' %% x2\n
#print 'b5: %%d' %% b5\n
print '%%d' %% ((b5 + 8) / 2 ** 4)\n
" | python - "$1"
}

allregs=()

case "$1" in
	read_regs)
		allregs+=( $(read_word 0xAA 5) )
		allregs+=( $(read_word 0xB4 6) )
		for i in 0 1 2 6 7 8 9 10 ; do
			allregs[$i]=$(convert_to_short ${allregs[$i]})
		done
		for i in "${allregs[@]}" ; do
			echo "$i"
		done
		;;
	read_ut)
		write_control 0xF4 0x2E | ./test-write
		sleep 0.005
		t=$(read_word 0xF6 1)
		calculate_temp "$t"
		;;

	read_up)
		write_control 0xF4 0x34
		sleep 0.005
		read_word 0xF6 1
		;;
esac
