using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
 
 
namespace AsyncSimpleGUIClient
{
    public partial class ChatClient : Form
    {
        Socket mainSock;
        IPAddress thisAddress;
        delegate void AppendTextDelegate(Control ctrl, string s);
        AppendTextDelegate _textAppender;
        string nameID; //ID
        CharClient2 ch2 = new CharClient2();
        public static String june;
 
 
 
 
 
        public ChatClient()
        {
            InitializeComponent();
            mainSock = new Socket(AddressFamily.InterNetwork, 
                SocketType.Stream, ProtocolType.Tcp);
 
            _textAppender = new AppendTextDelegate(AppendText);
        }
 
        
 
 
 
        void AppendText(Control ctrl, string s)
        {
            if (ctrl.InvokeRequired) ctrl.Invoke(_textAppender, ctrl, s);
            else
            {
                string source = ctrl.Text;
                ctrl.Text = source + Environment.NewLine + s;
            }
        }
        private void OnFormLoaded(object sender, EventArgs e)
        {
            //txtTTS.Text = "여기에다가 글자 입력받아야지";
            txtTTS.Text= june;
 
 
        }
 
        private void btnSend_Click(object sender, EventArgs e)
        {
            
           //harClient2.emoticon.Text = txtTTS.Text;
 
            if (!mainSock.IsBound)
            {
                //MsgBoxHelper.Warn("서버가 실행되고 있지 않습니다!");
                return;
            }
 
            // 보낼 텍스트
 
            string tts =  txtTTS.Text.Trim();
            txtTTS.Text = june ;
            if (string.IsNullOrEmpty(tts))
            {
                
                //MsgBoxHelper.Warn("텍스트가 입력되지 않았습니다!");
                txtTTS.Focus();
               
                return;
            }
            string[] token = tts.Split(':');
 
            if (token[0].Equals("BR"))
            {
                byte[] bDts = Encoding.UTF8.GetBytes
                    ("BR:"+nameID + ":" + token[1]+ june + ":");
                mainSock.Send(bDts);
            }
            else
            {
                byte[] bDts = Encoding.UTF8.GetBytes
                 ("TO:" + nameID + ":" + token[0] + ":"+token[1] + june + ":");
                mainSock.Send(bDts);
            }
 
        }
          
    
     
 
 
        void DataReceived(IAsyncResult ar)
        {
            AsyncObject obj = (AsyncObject)ar.AsyncState;
 
            // 데이터 수신을 끝낸다.
            int received = obj.WorkingSocket.EndReceive(ar);
 
            string text = Encoding.UTF8.GetString(obj.Buffer);
 
            string[] token = text.Split(':');
            string cmd = token[0];
            if (cmd.Equals("LOGIN_SUCC"))
            {
                AppendText(txtHistory, "로그인");
                panel4.BackColor = Color.Green;
 
            }
            else if (cmd.Equals("BR_SUCC"))
            {
                AppendText(txtHistory, "브로드캐스트 성공");
               
              
            }
            else if (cmd.Equals("To_SUCC"))
            {
                AppendText(txtHistory, "To 성공");
            }
            else if (cmd.Equals("TO"))
            {
                string fromID = token[1];
                string toID = token[2];
                string msg = token[3];
                AppendText(txtHistory, string.Format
                    ("[From: {0}][To: {1}] {2}", fromID, toID, msg));
            }
            else if (cmd.Equals("BR"))
            {
                AppendText(txtHistory, "[전체]" + token[1]+":"+token[2]);
            }
            else if (cmd.Equals("server"))
            {
                AppendText(txtHistory, "[공지]"+token[1]);
            }
            else
            {
                AppendText(txtHistory, text);
            }
 
 
            obj.ClearBuffer();
            obj.WorkingSocket.BeginReceive(obj.Buffer, 0, obj.BufferSize, 0, DataReceived, obj);
 
 
        }
 
      
 
 
 
            private void btnConnect_Click(object sender, EventArgs e)
        {
            
 
            if (mainSock.Connected)
            {
                //MsgBoxHelper.Error("이미 연결되어 있습니다!");
                return;
            }
            int port;
            if (!int.TryParse(txtPort.Text, out port))
 
            {
                txtPort.Focus();
                txtPort.SelectAll();
                return;
            }
            if (thisAddress == null)
            {
                thisAddress = IPAddress.Loopback;
                txtAddress.Text = thisAddress.ToString();
            }
            IPEndPoint serverEP = new IPEndPoint(thisAddress, port); //server
            try
            {
                mainSock.Connect(thisAddress, port);
            }
            catch // (Exception ex)
            {
                // MsgBoxHelper.Error("연결에 실패했습니다!\n오류 내용: {0}", MessageBoxButtons.OK, ex.Message);
                return;
            }
            // 서버로 ID  전송
            nameID = txtID.Text.Trim();
            AppendText(txtHistory,  "서버와 연결되었습니다");
            string msg = "ID:" + nameID+":";
            byte[] data = Encoding.UTF8.GetBytes(msg);
            mainSock.Send(data);
 
            AsyncObject obj = new AsyncObject(4096);
            obj.WorkingSocket = mainSock;
            mainSock.BeginReceive(obj.Buffer, 0, obj.BufferSize, 0, DataReceived, obj);
        }
 
 
        private void image_Click(object sender, EventArgs e)
        {
            CharClient2 ch2 = new CharClient2();
 
            ch2.Show();
 
 
        }
 
      
 
 
        private void ChatForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (mainSock != null) //      if (!mainSock.IsBound)
            {
                mainSock.Disconnect(false);
                mainSock.Close();
            }
        }
