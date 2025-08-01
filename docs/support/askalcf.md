# AskALCF: A RAG-empowered ChatBot for User Support

AskALCF is an intelligent, Retrieval-Augmented Generation (RAG) powered chatbot system designed to enhance user support at the Argonne Leadership Computing Facility (ALCF). It provides fast, accurate, and context-aware responses to user queries by leveraging a curated HPC-specific knowledge base and state-of-the-art language models.


![Figure 1: Retrieval Augmented Generation Pipeline](./figures/rag.png)  
*Retrieval Augmented Generation Pipeline: LLM retrieves relevant external documents before generating an answer, grounding its output in factual, up-to-date information.*

---

## Knowledge Base

Below are the documents curated in the vector database used by AskALCF ChatBot.

| Data                              | Type / format |
|-----------------------------------|---------------|
| ALCF User Guide               | Markdown      |
| OLCF User Guide             | RST           |
| LLNL User Guide            | Web           |
| PBS Documentation         | PDF           |
| Slurm Documentation        | Web           |
| CUDA C Programming Guide    | Web           |
| SYCL Documentation         | PDF           |
| AMD HIP Documentation        | Web           |
| Intel OneAPI Documentation  | Web           |

---

## Accessing AskALCF

The ChatBot is currently hosted on Crux and accessible via SSH tunneling. We are in the process of hosting a VM for public access without SSH tunneling. 

To access it:

1. Add the following to your `~/.ssh/config` file:
	```bash
	Host askalcf-user
		User usernamels
		HostName crux-uan-0001
		ProxyJump username@crux.alcf.anl.gov
		LocalForward 9505 localhost:9505
	```

2. Then SSH into the service: ``ssh askalcf-user``

3. Open a browser and go to: http://localhost:9505

![Figure 2: AskALCF ChatBot Web UI](./figures/chatbot.png)  
*AskALCF ChatBot Web UI* 

⸻

## Example Questions

Please make your question as specific as possible. This helps the ChatBot retrieve the most relevant information and provide a high-quality answer.
	•	On Aurora, what is the command to log in, and what is the default quota for user home directories?
	•	How do I request 8 Aurora nodes all within the same rack using PBS? Script: pbs_submit_script.sh, Project: RackTest, Queue: prod, Duration: 1 hour, Filesystem: grand.
	•	Will my ALCF HPSS home directory remain accessible for my group members after my account termination?
	•	What is the best environment variable setting for oneCCL?
	•	How should I use copper to run a Python job at scale on Aurora?
	•	What is the optimal CPU binding on Aurora?

⸻

## Providing Feedback

* If you have feedback on a specific query, you can directly rate and comment on the chatbot’s response:
  - Click “Rate this assistant response”
  - Select a rating
  - Optionally provide additional comments
  - Click “Submit rating”

  We only track queries, responses, and ratings. No personal information is collected.

* For high-level comments or feedbacks, please contact: 📧 support@alcf.anl.gov