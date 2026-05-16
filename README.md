# L²/t Page Replacement

**Author:** Stanislav Usychenko  
**arXiv paper:** [link after publication]  
**Formula:** viability = L² / (t + 1)

L²/t is a new page replacement policy for operating systems that combines **frequency (L)** and **age (t)** in a **nonlinear** way. It strongly protects frequently accessed pages while still evicting old pages.

## Results (MSR web trace, 500k accesses, cache 1000 pages)

| Policy | Hit Ratio |
|--------|------------|
| LRU    | 0.8124     |
| LFU    | 0.8451     |
| L²/t   | **0.8612** |

## Compile and run the simulator

```bash
make
./vmsim 1000 msr_trace.txt