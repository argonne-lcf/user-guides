program main

  double precision, allocatable :: a(:) 
  double precision, allocatable :: b(:) 
  double precision  scalar
  integer err
  integer num_errors
  integer num_elements

  scalar = 8d0
  num_errors = 0
  num_elements = 1024

  allocate (a(num_elements),stat=err)
  allocate (b(num_elements),stat=err)

  ! initialize on the host
  do j=1,num_elements
     a(i) = 0d0
     b(i) = j
  end do


!$omp target teams distribute parallel do simd map(tofrom:a(1:num_elements)) map(to:b(1:num_elements))
    do i=1,num_elements
       a(j) = a(j)+scalar*b(j)
    end do
!$omp end target teams distribute parallel do simd


  ! error checking
  do j=1,num_elements
     if( a(i) - j*scalar .gt. 0.000001 ) then
        num_errors = num_errors + 1
     end if

  end do

  deallocate(a);
  deallocate(b);

  if(num_errors == 0) write(*,*) "Success!\n"

end program main
