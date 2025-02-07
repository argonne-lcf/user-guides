module foo
  use iso_c_binding
  implicit none

  interface
    double precision function c_offload(me,num_procs) bind(C, name="offload")
      use iso_c_binding
      integer (c_int), value :: me, num_procs
    end function c_offload
  end interface
end module foo

program main
  use iso_c_binding
  use foo
  implicit none
  include 'mpif.h'

  integer :: me, num_procs
  integer :: provided, ierr
  double precision :: res_, res

  call MPI_Init_thread(MPI_THREAD_SINGLE, provided, ierr)
  call MPI_Comm_rank(MPI_COMM_WORLD, me, ierr)
  call MPI_Comm_size(MPI_COMM_WORLD, num_procs, ierr)

  res_ = c_offload(me, num_procs)

  call MPI_Reduce(res_, res, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD, ierr)

  if(me == 0) then
  if(res < 1e-6) then
    write(*,*) 'Result is CORRECT!!! :)'
  else
    write(*,*) 'Result is WRONG!!! :('
  endif
  endif

  call MPI_Finalize(ierr)

end program main

