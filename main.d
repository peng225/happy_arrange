import std.stdio;
import std.container;
import std.bitmanip;

int main(string[] args)
{
  //コマンドライン引数の数をチェック  
  // 志望数と各志望度のスコアをセット

  
  immutable int NUM_CHOICES;
  immutable int[NUM_CHOICES] SCORES;
  
  // 入力ファイルを開く
  
  // 人数を読み込む
  immutable int NUM_PEOPLE;
  // 部署数を読み込む
  immutable int NUM_DEPT;

  // SList(BitArray[NUM_DEPT]) choices_list;
  // int[NUM_DEPT] dept_capacity;
   
  for(int i = 0; i < NUM_DEPT; i++){
    // 各部署の定員を読み込む
  }

  for(int i = 0; i < NUM_PEOPLE; i++){
    for(int j = 0; j < NUM_CHOICES; j++){
      // 各人のj番目の志望を読み込む
    }
  }

  // 全志望度ベクトルの重心を計算

  // 幅優先探索のためのqueueに空のルートノードを入れる

  // 探索深さが人数より小さい間
  {
    // これまでに選択したベクトルの数が0なら
    {
      // 全志望度ベクトルの重心に最も近い志望度ベクトルを選ぶ
    }
    // さもなければ
    {
      // これまでに選択したベクトルの重心に最も近い志望度ベクトルを選ぶ
    }    

    // queueが空でなく、かつノードをqueueから取り出したとき、その深さがdである間
    {
      for(int i = 0; i < NUM_DEPT; i++){
	// 取り出したノードよりも深さを１つ増やした新規ノード情報を作成
	// 現在注目している人がi番目の部署を選んだという情報を新規ノードに加える
	// もしこれまでに選択された部署の集合が持つスコアが既存のものよりも高い場合
	{
	  // 自分が持つ部署集合のスコアを記録
	  // 新規ノードをqueueに入れる
	}
      }
    }
    
    // 今回選ばれた志望度ベクトルを、 選択済みベクトルリストに移す
    // これまでに選ばれた志望度ベクトルの重心を計算
  }

  for(int i = 0; i < NUM_PEOPLE; i++){
    // 各人の配属先を表示
  }
  // 空行を挿入
  // 全体の幸福度を表示
}