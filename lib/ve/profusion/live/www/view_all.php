<?
require_once("pfcmu.inc");

$urls = array();
for($i=0 ; $i<20 ; $i++) {
  $urls[] = sprintf("http://localhost/ping.php?hostname=ve%02d", $i);
}

$alive = fetchall($urls);

?>
<html>
<header>
<script type="text/javascript" src="reload.js"></script>
<style type-"text/css">
<!--
td {
  width: 160px;
  height: 120px;
  border-width: 0px;
  padding: 4px 4px 4px 4px;
  text-align: center;
  background-color: #888;
}
img {
  border: none;
}
a {
  border: none;
  text-decoration: none;
}
-->
</style>
</header>
<body>
<table>
<?
for($y=0 ; $y<4 ; $y++) {
  echo "<tr>";
  for($x=1 ; $x<=5 ; $x++) {
    $c = $y * 5 + $x;
    if( $alive[$c] ) {
      printf("<td id=\"ve%dtd\"><a href=\"view_pf.php?pf=%d\">%s</a><br>ve%02d</td>\n", $c, $c, live("ve$c", sprintf("/live/ve%02d/thumb.jpg", $c), 50, 160, 120), $c);
    } else {
      printf("<td>ve%02d is inactive</td>\n", $c);
    }
  }
  echo "</tr>\n";
}
?>
</table>
</body>
</html>

