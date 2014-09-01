#include "arrange.h"


double getScoreMean(const vector<int> &scores, int numDept)
{
  accumulator_set<int, stats<tag::mean> > acc;  

  // scoresには希望が通らなかった場合のスコア(0)も含まれるので、それを除く
  for(int i = 0; i < (int)(numDept - scores.size()) + 1; i++){
    acc(0);
  }

  for(vector<int>::const_iterator i = begin(scores);
      i != end(scores); i++){
    acc(*i);
  }
  
  return mean(acc);
}

double getScoreVariance(const vector<int> &scores, int numDept)
{
  accumulator_set<int, stats<tag::variance> > acc;  

  // scoresには希望が通らなかった場合のスコア(0)も含まれるので、それを除く
  for(int i = 0; i < (int)(numDept - scores.size()) + 1; i++){
    acc(0);
  }

  for(vector<int>::const_iterator i = begin(scores);
      i != end(scores); i++){
    acc(*i);
  }
  
  return variance(acc);
}

void sortFollowerWithMaster(vector<int>::iterator m_b,
			    vector<int>::iterator m_e,
			    vector<int>::iterator f_b,
			    vector<int>::iterator f_e)
{
  if(distance(m_b, m_e) != distance(f_b, f_e)){
    cerr << "Error: Length of master and follower must be the same." << endl;
    exit(1);
  }
  
  if(distance(m_b, m_e) <= 1){
    return;
  }

  vector<int>::iterator m_l  = m_b;
  vector<int>::iterator m_r = m_e - 1;
  vector<int>::iterator f_l  = f_b;
  vector<int>::iterator f_r = f_e - 1;
  int pivot = *m_r;

  while(distance(m_l, m_r) > 0){
    while(distance(m_l, m_r) > 0 && *m_l < pivot){
      ++m_l;
      ++f_l;
    }
    if(distance(m_l, m_r) == 0){
      break;
    }

    while(distance(m_l, m_r) > 0 && pivot <= *m_r){
      --m_r;
      --f_r;
    }
    if(distance(m_l, m_r) == 0){
      break;
    }

    iter_swap(m_l, m_r);
    iter_swap(f_l, f_r);
  }

  iter_swap(m_l, m_e - 1);
  iter_swap(f_l, f_e - 1);

  sortFollowerWithMaster(m_b, m_l, f_b, f_l);
  sortFollowerWithMaster(m_l + 1, m_e, f_l + 1, f_e);
}

void sortFollowerWithMaster(vector<vector<int> >::iterator m_b,
			    vector<vector<int> >::iterator m_e,
			    vector<int>::iterator f_b,
			    vector<int>::iterator f_e)
{
  if(distance(m_b, m_e) != distance(f_b, f_e)){
    cerr << "Error: Length of master and follower must be the same." << endl;
    exit(1);
  }
  
  if(distance(m_b, m_e) <= 1){
    return;
  }

  vector<vector<int> >::iterator m_l  = m_b;
  vector<vector<int> >::iterator m_r = m_e - 1;
  vector<int>::iterator f_l  = f_b;
  vector<int>::iterator f_r = f_e - 1;
  vector<int> pivot = *m_r;

  while(distance(m_l, m_r) > 0){
    while(distance(m_l, m_r) > 0 && *m_l < pivot){
      ++m_l;
      ++f_l;
    }
    if(distance(m_l, m_r) == 0){
      break;
    }

    while(distance(m_l, m_r) > 0 && pivot <= *m_r){
      --m_r;
      --f_r;
    }
    if(distance(m_l, m_r) == 0){
      break;
    }

    iter_swap(m_l, m_r);
    iter_swap(f_l, f_r);
  }

  iter_swap(m_l, m_e - 1);
  iter_swap(f_l, f_e - 1);

  sortFollowerWithMaster(m_b, m_l, f_b, f_l);
  sortFollowerWithMaster(m_l + 1, m_e, f_l + 1, f_e);
}

double getCutOffLowerBound(list<Node> &q,
			   double scoreMean, double scoreVariance,
			   int d, int numPeople)
{
  assert(numPeople != 1);

  accumulator_set<int, stats<tag::variance, tag::mean> > acc;
  BOOST_FOREACH(Node n, q) {    
    acc(n.getScore());
  }
  double mn = mean(acc);
  double va = variance(acc);
  double specMn = mn + scoreMean;
  double specSd = sqrt(va + scoreVariance);
  
  return specMn - 2.0 * specSd / (d + 1);
}

// vector<int> computeCenter(const vector<vector<int> > &choices)
// {
//   assert(choices.size() != 0);
//   vector<int> center(choices.at(0).size());

//   for(int j = 0; j < (int)center.size(); j++){
//     int sum = 0;
//     for(vector<vector<int> >::const_iterator i = begin(choices);
// 	i != end(choices); i++){
//       sum += i->at(j);
//     }
//     center.at(j) = sum / choices.size();
//   }
  
//   return center;
// }

void rmInferiorNodes(list<Node> &q, const map<vector<int>, int> &score_table)
{
  assert(!q.empty());
  for(list<Node>::iterator i = begin(q);
      i != end(q); i++){
    if(score_table.find(i->getDepts()) != end(score_table)
       && i->getScore() < score_table.at(i->getDepts())){
      i = q.erase(i);
      i--;
    }
  }
}

// pair<int, vector<int> >
// getNearestVector(const vector<int> &center,
// 		 vector<vector<int> >::const_iterator b,
// 		 vector<vector<int> >::const_iterator e)
// {
//   pair<int, vector<int> > target_info;
//   target_info.second.resize(center.size());

//   double dist = std::numeric_limits<double>::max();
//   for(vector<vector<int> >::const_iterator i = b;
//       i != e; i++){
//     double tmp_dist = computeDistance(*i, center);
//     if(tmp_dist < dist){
//       dist = tmp_dist;      
//       target_info.first = distance(b, i);
//       target_info.second = *i;
//     }
//   }

//   return target_info;
// }

list<Node> pdpSearch(const vector<int> &scores,
		     vector<int> &capacity,
		     vector<vector<int> > &choices,
		     vector<int> &choicesID,
		     bool verbose,
		     bool hoplessCut)
{
  // 全志望度ベクトルの重心を計算  
  // vector<int> center(scores.size());
  // center = computeCenter(choices);
  // cout << "center:" << endl;
  // showVector(center);
  
  // 枝狩り幅優先探索のためのlistに空のルートノード(深さ0)を入れる
  Node root(0, choices.front().size());
  list<Node> q;
  q.push_back(root);

  // これまでに選択した部署の集合をキーとし、スコアをバリューとしたマップ
  map<vector<int>, int> score_table;        
  double scoreMean = getScoreMean(scores, choices.front().size());
  double scoreVariance = getScoreVariance(scores, choices.front().size());
  
  // 探索深さdが人数より小さい間
  for(int d = 0; d < (int)choices.size(); d++) {
    if(verbose){
      cout << endl;    
      cout << "depth: " << d << endl;
    }
    // 全志望度ベクトルの重心に最も近い志望度ベクトルを選ぶ
    // pair<int, vector<int> > targetInfo
    //   // = getNearestVector(center, begin(choices) + d, end(choices));
    //   = getNearestVector(center, begin(choices) + d, begin(choices)+d+1);
    // // これから注目するベクトル
    // vector<int> target = targetInfo.second;

    // // 選択したベクトルをchoicesから削除
    // // choices.erase(begin(choices) + targetInfo.first);
    // swap(choices.at(d), choices.at(d + targetInfo.first));
    // swap(choicesID.at(d), choicesID.at(d + targetInfo.first));

    vector<int> target = choices.at(d);

    if(verbose){
      cout << choicesID.at(d) << ": ";
      showVector(target);
    }

    // 実際の処理に入る前に劣っているノードを削除する
    rmInferiorNodes(q, score_table);
    
    // rmHopelessNodes(q, d, choices.size());

    /*
      カットオフ値をここで決めておく。
      この値は現在のqの内容から推測する。
    */
    double cutOff = 0;
    if(hoplessCut){
      cutOff = getCutOffLowerBound(q, scoreMean, scoreVariance,
			    d, choices.size());
    }
    
    int numRemoved = 0;

    assert(!q.empty());
    cout << "d, q: " << d << ", " << q.size() << endl;

    // 先頭要素の深さがdである間
    while(q.front().getDepth() == d){
      assert(!q.empty());
      // if(q.front().getScore() < score_table[q.front().getDepts()]){
      // 	q.erase(begin(q));
      // 	continue;
      // }
      Node node = q.front();
      q.erase(begin(q));      
      for(int i = 0; i < (int)choices.front().size(); i++){
	// 部署iがすでに定員に達していたら
	if(node.getNumDept(i) == capacity.at(i)){
	  continue;
	}
	// 取り出したノードよりも深さを１つ増やした新規ノード情報を作成
	Node newNode = node;
	newNode.incrementDepth();
	// 現在注目している人がi番目の部署を選んだという情報を新規ノードに加える
	newNode.addDept(i);
	
	// もしこれまでに選択された部署の集合が未登録なら登録	
	if(score_table.find(newNode.getDepts()) == end(score_table)){
	  if(!hoplessCut || cutOff <= (node.getDepth() == 0 ? 
			0 : score_table.at(node.getDepts()))
	     + target.at(i)){
	    // 履歴を更新
	    newNode.addHistory(i);
	    // 自分が持つ部署集合のスコアを記録
	    // cout << "i: " << i << endl;
	    if(verbose){
	      cout << "new: ";
	      showVector(newNode.getHistory());
	    }

	    if(node.getDepth() == 0){
	      score_table[newNode.getDepts()] = target.at(i);
	    }else{
	      score_table[newNode.getDepts()]
		= score_table.at(node.getDepts()) + target.at(i);
	    }
	    newNode.setScore(score_table.at(newNode.getDepts()));
	  
	    if(verbose){
	      cout << "new score: " << newNode.getScore() << endl;
	    }
	  
	    // 新規ノードをlistに入れる
	    q.push_back(newNode);	    
	  }else{
	    numRemoved++;
	  }
	}
	// 既存のものよりスコアが高い場合は登録	  
	else if(score_table.at(newNode.getDepts())
		< score_table.at(node.getDepts()) + target.at(i)){
	  if(!hoplessCut || cutOff <= score_table.at(node.getDepts()) + target.at(i)){
	    // 履歴を更新
	    newNode.addHistory(i);
	    // 自分が持つ部署集合のスコアを記録
	    if(verbose){
	      cout << "upd: ";
	      showVector(newNode.getHistory());
	    }
	    score_table[newNode.getDepts()]
	      = score_table.at(node.getDepts()) + target.at(i);
	    newNode.setScore(score_table.at(newNode.getDepts()));
	    if(verbose){
	      cout << "upd score: " << newNode.getScore() << endl;
	    }
	    // 新規ノードをlistに入れる
	    q.push_back(newNode);	    
	  }else{
	    numRemoved++;
	  }
	}
      }
    }
    
    cout << "rm: " << numRemoved << endl;
  }
  return q;
}

void pdpSelect(list<Node> &q,	  
	  vector<int> &result,
	  int &score)
{
  score = 0;
  while(!q.empty()){
    Node node = q.front();
    q.erase(begin(q));    
    if(score < node.getScore()){
      score = node.getScore();
      result = node.getHistory();
    }
  }  
}
