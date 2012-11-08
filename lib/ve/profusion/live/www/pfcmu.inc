<?
function fetchall($urls, $timeout=0) {
  $mh = curl_multi_init();
  foreach($urls as $i => $url) {
    $conn[$i] = curl_init($url);
    curl_setopt($conn[$i], CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($conn[$i], CURLOPT_FAILONERROR, 1);
    curl_setopt($conn[$i], CURLOPT_FOLLOWLOCATION, 1);
    curl_setopt($conn[$i], CURLOPT_MAXREDIRS, 3);

    # ignore SSL validation
    curl_setopt($conn[$i], CURLOPT_SSL_VERIFYPEER, false);
    curl_setopt($conn[$i], CURLOPT_SSL_VERIFYHOST, false);

    if($timeout > 0) {
      curl_setout($conn[$i], CURLOPT_TIMEOUT, $timeout);
    }
    
    curl_multi_add_handle($mh, $conn[$i]);
  }

  $active = null;
  do {
    $mrc = curl_multi_exec($mh, $active);
  } while ($mrc == CURLM_CALL_MULTI_PERFORM);

  while($active and $mrc == CURLM_OK) {
    if(curl_multi_select($mh) != -1) {
      do {
        $mrc = curl_multi_exec($mh,$active);
      } while ($mrc == CURLM_CALL_MULTI_PERFORM);
    }
  }

  if($mrc != CURLM_OK) {
    echo "Read error: " . $mrc;
  }

  $res = array();
  foreach($urls as $i => $url) {
    if(($err = curl_error($conn[$i])) == '') {
      $res[$i] = curl_multi_getcontent($conn[$i]);
    } else {
      echo "Cannot get: " . $urls[$i] . "<br/>";
    }
    curl_multi_remove_handle($mh, $conn[$i]);
    curl_close($conn[$i]);
  }

  curl_multi_close($mh);

  return $res;
}

function live($id, $url, $msec, $width=0, $height=0, $opt="") {
  if($width > 0) {
    $opt .= " width=\"$width\"";
  }
  if($height > 0) {
    $opt .= " height=\"$height\"";
  }
  //return "<img src=\"$url\" alt=\"\" name=\"$id\" onload='setTimeout(function(){RefreshImg(\"$id\", \"$url\");}, $msec)' onerror='RefreshImg(\"$id\", \"$url\")' onabort='setTimeout(function(){RefreshImg(\"$id\", \"$url\");}, $msec)' id=\"$id\" $opt>";
  return "<img src=\"$url\" alt=\"\" name=\"$id\" onload='InitialImg(\"$id\", \"$url\")' id=\"$id\" $opt>";
}

?>
