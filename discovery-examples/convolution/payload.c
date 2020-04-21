int payload(int n) {
  if (n == 0) return(1);
  if (n == 1) return(1);

  return(payload(n-1))+payload(n-2)+1;
}
