./ipc_perf_main_sig 10 signals > sig.out
./ipc_perf_main_sig 50 signals >> sig.out
./ipc_perf_main_sig 100 signals >> sig.out
./ipc_perf_main_sig 150 signals >> sig.out
./ipc_perf_main_sig 200 signals >> sig.out
./ipc_perf_main_comm 10 pipe > comm.out
./ipc_perf_main_comm 50 pipe >> comm.out
./ipc_perf_main_comm 100 pipe >> comm.out
./ipc_perf_main_comm 150 pipe >> comm.out
./ipc_perf_main_comm 200 pipe >> comm.out
./ipc_perf_main_comm 10 FIFO >> comm.out
./ipc_perf_main_comm 50 FIFO >> comm.out
./ipc_perf_main_comm 100 FIFO >> comm.out
./ipc_perf_main_comm 150 FIFO >> comm.out
./ipc_perf_main_comm 200 FIFO >> comm.out
./ipc_perf_main_comm 10 lsock >> comm.out
./ipc_perf_main_comm 50 lsock >> comm.out
./ipc_perf_main_comm 100 lsock >> comm.out
./ipc_perf_main_comm 150 lsock >> comm.out
./ipc_perf_main_comm 200 lsock >> comm.out
./ipc_perf_main_comm 10 socket >> comm.out
./ipc_perf_main_comm 50 socket >> comm.out
./ipc_perf_main_comm 100 socket >> comm.out
./ipc_perf_main_comm 150 socket >> comm.out
./ipc_perf_main_comm 200 socket >> comm.out
