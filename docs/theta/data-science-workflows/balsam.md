# Balsam

Balsam is a project run by the ALCF Data Science group, to optimize workflow execution on ALCF systems (and elsewhere). With Balsam, users can easily describe a large campaign of jobs to be run, with varying sizes and interdependencies, and let Balsam manage flowing the jobs out to the target systems. Balsam will stage in necessary inputs, submit jobs to the queues, and stage out specified output files. Users can also control the flow of jobs to prioritize particular workflows or jobs. 

Balsam was originally developed to support ATLAS workflows on ALCF systems, linking a serial preprocessing stage with large-scale parallel execution on Mira, followed by serial post-processing.

- Documentation: [https://balsam.readthedocs.io/en/latest](https://balsam.readthedocs.io/en/latest)
- Source repository: [https://github.com/argonne-lcf/balsam](https://github.com/argonne-lcf/balsam)

**Note:** All users of the Theta Balsam module with a database created before February 17, 2021, must take action to continue using the module. See this page for details. No action is needed if you have been using your own installation of Balsam in a virtual environment.

==add video==

