module foo
contains
  attributes (global) subroutine addKernel(c, a, b, size)
    implicit none
    double precision,device :: c(*), a(*), b(*)
    integer,device :: i, size
    i = blockDim%x * (blockIdx%x - 1) + threadIDx%x
    if(i <= size) c(i) = a(i) + b(i)
    return
  end subroutine addKernel
end module foo

subroutine _CUDA_CHECK_ERRORS(err)
  implicit none
  integer :: err
  ! -- check error...
end subroutine

program main
  use cudafor
  use foo
  implicit none
  include 'mpif.h'
  integer, parameter :: SIZE = 1024
  integer :: i
  integer, device :: i_dev
  double precision, allocatable :: c(:), a(:), b(:)
  double precision, allocatable, device :: c_dev(:), a_dev(:), b_dev(:)
  double precision :: diff
  type(dim3) :: grid, tblock

  integer :: istat, len, num_devices, device_id
  type(cudadeviceprop) :: prop

  integer :: me, num_procs
  integer :: provided, ierr

  call MPI_Init_thread(MPI_THREAD_SINGLE, provided, ierr)
  call MPI_Comm_rank(MPI_COMM_WORLD, me, ierr)
  call MPI_Comm_size(MPI_COMM_WORLD, num_procs, ierr)

  ! -- Initialize host

  allocate(c(SIZE))
  allocate(b(SIZE))
  allocate(a(SIZE))
  
  allocate(c_dev(SIZE))
  allocate(b_dev(SIZE))
  allocate(a_dev(SIZE))
  
  do i=1, SIZE
     a(i) = sin(real(i)) * sin(real(i))
     b(i) = cos(real(i)) * cos(real(i))
     c(i) = -1.0
  end do

  ! -- Number of total work items

  tblock = dim3(64,1,1)
  grid = dim3(ceiling(real(SIZE)/tblock%x),1,1)

  ! -- Cuda Properties

  istat = cudaGetDeviceCount(num_devices)
  call _CUDA_CHECK_ERRORS(istat)

  if(me == 0) write(*,*) '# of devices= ',num_devices

  ! -- Device ID

  device_id = mod(me, num_devices)
  write(*,*) 'Rank ',me,' running on GPU ',device_id

  istat = cudaGetDeviceProperties(prop, device_id)
  call _CUDA_CHECK_ERRORS(istat)

  if(me == 0) then
  len = verify(prop%name, ' ', .true.)
  write(*,*) 'Name= ',prop%name(1:len)
  write(*,*) 'Locally unique identifier= ', prop%luid

  write(*,*) 'Clock Frequency(KHz)= ',real(prop%clockRate)
  write(*,*) 'Major compute capability= ',prop%major
  write(*,*) 'Minor compute capability= ',prop%minor

  write(*,*) 'Number of multiprocessors on device= ',prop%multiProcessorCount

  write(*,*) 'Warp size in threads= ',prop%warpSize
  write(*,*) 'Single precision performance ratio= ',prop%singleToDoublePrecisionPerfRatio
  endif

  ! -- allocate & transfer data to device
  i_dev = i
  c_dev = c
  a_dev = a
  b_dev = b

  ! -- launch kernel
  call addKernel<<<grid,tblock>>>(c_dev, a_dev, b_dev, i_dev)

  ! -- transfer data to host
  c = c_dev

  diff = 0
  do i=1, SIZE
    diff = diff + c(i)
  end do
  diff = diff / real(SIZE) - 1.0

  if(diff*diff < 1e-6) then
     write(*,*) 'Result is CORRECT!! :)'
  else
     write(*,*) 'Result is WRONG!! :('
  endif 

  deallocate(a_dev)
  deallocate(b_dev)
  deallocate(c_dev)
  
  deallocate(a)
  deallocate(b)
  deallocate(c)

  call MPI_Finalize(ierr)

end program main

