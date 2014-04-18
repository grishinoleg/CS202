import random
from collections import deque

process_length = 15
max_processes = 10
run_params = [10, 100, 20]
diskread_params = [10, 30, 10]
semaphore_params = [0, 3, 1]
pids = deque([1,2])
available_pids = deque([x for x in range(3, max_processes)])
events_list = ["run", "diskread", "keyboardread", "diskwrite", "down", "up", "fork"]
events_prob = [[0.3, 0.6, 0.8, 0.85, 0.9, 0.95, 1], [0.6, 0.7, 0.8, 0.85, 0.9, 0.95, 1], [0.5, 0.6, 0.7, 0.8, 0.9, 1, 1]]

def main():
	with open("processes.dat", "w") as f:
		f.write("0 fork 1\n")
		f.write("0 fork 2\n")
		while len(pids) != 0 and len(available_pids) != 0:
			pid = pids.popleft()
			for event in range(process_length):
				new_event_p = random.random()
				i = 0
				while (new_event_p>events_prob[pid%len(events_prob)][i]):
					i += 1
				new_event = events_list[i]
				event_str = str(pid) + " " + new_event
				if new_event == "fork":
					if len(available_pids) == 0:
						event_str = str(pid) + " run " + str(random.randrange(*run_params))
					else:
						new_pid = available_pids.popleft()
						pids.append(new_pid)
						event_str += " " + str(new_pid)
				elif new_event == "run":
					event_str += " " + str(random.randrange(*run_params))
				elif new_event == "diskread":
					event_str += " " + str(random.randrange(*diskread_params))
				elif new_event == "keyboardread":
					pass
				elif new_event == "diskwrite":
					pass
				elif new_event == "up" or new_event == "down":
					event_str += " " + str(random.randrange(*semaphore_params))
				f.write(event_str+"\n")
				print(event_str)

main()


