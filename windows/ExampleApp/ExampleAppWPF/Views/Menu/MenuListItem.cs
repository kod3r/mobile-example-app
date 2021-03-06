using Newtonsoft.Json.Linq;
using System.ComponentModel;

namespace ExampleAppWPF
{
    public class MenuListItem : INotifyPropertyChanged
    {
        private bool m_isSelected;
        private bool m_justRemoved;
        private int m_zIndex;
        private string m_heading;
        private bool m_isExpanded;

        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }

        public bool IsSelected
        {
            get { return m_isSelected; }
            set
            {
                if (value != m_isSelected)
                {
                    m_isSelected = value;
                    this.OnPropertyChanged("IsSelected");
                }
            }
        }

        public MenuListItem(string json, bool isExpanded, int zIndex)
        {
            m_zIndex = zIndex;            
            JObject parsed = JObject.Parse(json);
            m_heading = parsed["name"].Value<string>();
            IsExpanded = isExpanded;
        }

        public int ZIndex
        {
            get { return m_zIndex; }
        }

        public bool IsExpanded
        {
            get { return m_isExpanded; }
            set
            {
                if (m_isExpanded != value)
                {
                    m_isExpanded = value;
                    OnPropertyChanged("IsExpanded");
                }
            }
        }

        public bool JustAdded { get; set; }
        public bool JustRemoved
        {
            get
            {
                return m_justRemoved;
            }
            set
            {
                if (value != m_justRemoved)
                {
                    m_justRemoved = value;

                    OnPropertyChanged("JustRemoved");
                }
            }
        }

        public string Heading
        {
            get { return m_heading; }
        }
    }
}