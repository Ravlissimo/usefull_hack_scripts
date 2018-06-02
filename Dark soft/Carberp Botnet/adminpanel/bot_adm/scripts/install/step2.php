"<?php echo $lang['py']; ?>"
<br /><br />
<?php
if($_GET['go'] != 'index') exit;

$INSTALL = false;

define(__DIR__, str_replace(DIRECTORY_SEPARATOR . 'scripts' . DIRECTORY_SEPARATOR . 'install', '', dirname(__FILE__)));

function recurse($dir){
	if(is_file($dir)){
					echo $dir . ': <span style="color:red">'.$lang['nd'].'</span><hr />';
				}
			}
		}
	}elseif(is_dir($dir)){
		foreach($d as $value){
						echo $dir . $value . ': <span style="color:red">'.$lang['nd'].'</span><hr />';
					}
				}
            }

			if($value != '.' && $value != '..'){
				recurse($dir . $value . '/');
			}

		}
	}
}

recurse(__DIR__ . '/cache/');
recurse(__DIR__ . '/logs/');
recurse(__DIR__ . '/scripts/');
recurse(__DIR__ . '/crons/');
recurse(__DIR__ . '/classes/');
recurse(__DIR__ . '/modules/');
recurse(__DIR__ . '/cfg/');
recurse(__DIR__ . '/templates/');
recurse(__DIR__ . '/includes/');
recurse(__DIR__ . '/includes/config.php');

?>
<br />
<?php
if($INSTALL != true){
	}
?>
<?php echo $lang['pf']; ?>
<hr />
<input type="button" value="<?php echo $lang['next']; ?>" onclick="location = '/install/index.html?step=3';" />
<?php
}else{
?>
<?php echo $lang['pn']; ?><br /><br />"<span style="color:red"><?php echo $lang['nk']; ?></span>"!
<?php
}
?>