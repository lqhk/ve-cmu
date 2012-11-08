<?
// return true if alive

function alive($hostname) {
  exec("ping -c 1 -w 1 $hostname",&$output,&$ret);
  return $ret == 0;
}

echo alive($_GET['hostname']) ? 1 : 0;
?>
